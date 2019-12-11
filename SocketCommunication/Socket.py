import websocket 
import json

ws = websocket.websocket()

ws.connect("ws://192.168.0.25/")

myDict = {"sensor": "temperature", "identifier":"SENS123456789", "value":10, "timestamp": "20/10/2017 10:10:25"}

ws.send(json.dumps(myDict))

result = ws.recv()
print(result)

ws.close()
