import socket
import threading
from pymongo import MongoClient
import os

# MongoDB setup
mongo_user = os.getenv("MONGO_USER", "RafayKhan")
mongo_pass = os.getenv("MONGO_PASS", "Rafay321")
mongo_uri = f"mongodb+srv://{mongo_user}:{mongo_pass}@medicine.22ld5.mongodb.net/"
client = MongoClient(mongo_uri)
db = client.medicine_database
medicine_collection = db.medicines

# Function to handle client requests
def handle_client(client_socket):
    try:
        while True:
            request = client_socket.recv(1024).decode()
            if not request:
                break

            command, data = request.split(":", 1)
            if command == "ADD":
                name, quantity = data.split(",")
                name = name.strip()
                quantity = int(quantity.strip())

                # Check if medicine exists
                existing_medicine = medicine_collection.find_one({"name": name})
                if existing_medicine:
                    # Update the quantity if medicine exists
                    new_quantity = existing_medicine["quantity"] + quantity
                    medicine_collection.update_one({"name": name}, {"$set": {"quantity": new_quantity}})
                    client_socket.send(f"Medicine '{name}' updated. New quantity: {new_quantity}".encode())
                else:
                    # Add new medicine if it doesn't exist
                    medicine_collection.insert_one({"name": name, "quantity": quantity})
                    client_socket.send(f"Medicine '{name}' added with quantity: {quantity}".encode())

            elif command == "SEARCH":
                name = data.strip()
                medicine = medicine_collection.find_one({"name": name})
                if medicine:
                    response = f"Medicine found: {medicine['name']} (Quantity: {medicine['quantity']})"
                else:
                    response = "No medicine found."
                client_socket.send(response.encode())

            elif command == "DELETE":
                name = data.strip()
                result = medicine_collection.delete_one({"name": name})
                if result.deleted_count > 0:
                    client_socket.send(f"Medicine '{name}' deleted successfully.".encode())
                else:
                    client_socket.send(f"Medicine '{name}' not found.".encode())

            elif command == "SELL":
                name, quantity = data.split(",")
                name = name.strip()
                quantity = int(quantity.strip())

                # Check if medicine exists
                existing_medicine = medicine_collection.find_one({"name": name})
                if existing_medicine:
                    current_quantity = existing_medicine["quantity"]
                    if current_quantity >= quantity:
                        new_quantity = current_quantity - quantity
                        medicine_collection.update_one({"name": name}, {"$set": {"quantity": new_quantity}})
                        client_socket.send(f"Medicine '{name}' sold. Remaining quantity: {new_quantity}".encode())
                    else:
                        client_socket.send(f"Not enough stock for medicine '{name}'. Current quantity: {current_quantity}".encode())
                else:
                    client_socket.send(f"Medicine '{name}' not found.".encode())

            else:
                client_socket.send("Invalid command.".encode())
    except Exception as e:
        client_socket.send(f"Error processing request: {str(e)}".encode())
    finally:
        client_socket.close()

# Main server
def start_server():
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.bind(("0.0.0.0", 12345))
    server_socket.listen(5)
    print("Server started. Waiting for connections...")
    while True:
        client_socket, addr = server_socket.accept()
        print(f"Connection from {addr}")
        client_thread = threading.Thread(target=handle_client, args=(client_socket,))
        client_thread.start()

if __name__ == "__main__":
    start_server()
