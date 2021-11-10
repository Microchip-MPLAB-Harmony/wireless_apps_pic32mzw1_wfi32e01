import asyncio
import websockets,time

#user can change the default uri here
uri_default = "ws://192.168.103.12:8000"

async def send_data():    
    uri  = input("Enter the uri of the server, for example; ws://192.168.225.12:8000   : ")
    if (uri):
        pass
    else:
        uri=uri_default
    async with websockets.connect(uri) as websocket:
        
        data = input("Enter the data to send: ")
        while(1):
            await websocket.send(data)
            print(f">>> {data}")
            reply = await websocket.recv()
            print(f"Reply from server : {reply}")
            time.sleep(.5)
            data = input("Enter the data to send or enter exit() to exit  :")
            if data == "exit()":
                break
			
			
async def ping():
    
    uri  = input("Enter the uri of the server, for example; ws://192.168.225.12:8000   : ")
    if (uri):
        pass
    else:
        uri=uri_default
    async with websockets.connect(uri) as websocket:
        
        websocket.ping()
        reply = await websocket.recv()
        print(f"<<< {reply}")

		
asyncio.run(send_data())