import tkinter as Tk
import socket
import threading
import json

class ClientGui:
    def __init__(self, master, cmd_start, cmd_stop):   
        self.start_button_val = Tk.IntVar()
        self.serverIP = Tk.StringVar()
        self.serverPort = Tk.StringVar()
        self.serverIP.set("192.168.0.34")
        self.serverPort.set("5555")
        self.start_button = Tk.Checkbutton(master, text="Connect to\nServer", command=lambda: self.manage_start_button(cmd_start, cmd_stop), font="Arial 12", variable=self.start_button_val, indicatoron=False, width=10, height=3)        
        self.IP_label_name = Tk.Label(master, text="TCP Server IP", width=15, font="Arial 12")
        vcmd1 = master.register(self.validateIP) # we have to wrap the command
        self.IP_entry_value = Tk.Entry(master, width=15, font="Arial 12", validate="key", validatecommand=(vcmd1, '%S','%P'), textvariable=self.serverIP)
        self.Port_label_name = Tk.Label(master, text="TCP Server Port", width=15, font="Arial 12")
        vcmd2 = master.register(self.validatePort) # we have to wrap the command
        self.Port_entry_value = Tk.Entry(master, width=10, font="Arial 12", validate="key", validatecommand=(vcmd2,'%S', '%P'), textvariable=self.serverPort)              
        
        self.qtFrame = Tk.LabelFrame(master, text="QT7 Xpro Simulator", width=400, height=400, bd=1, relief="ridge") 

        self.sliderVal = Tk.DoubleVar()
        self.sliderWid = Tk.Scale(self.qtFrame, variable = self.sliderVal, font="Arial 15", from_ = 0, to = 255, orient = Tk.HORIZONTAL, 
                            sliderlength = 30, width = 57, length = 300, state = "disabled", showvalue = 0)
        
        self.sliderBt1Val = Tk.IntVar()
        self.sliderBt1 = Tk.Checkbutton(self.qtFrame, text="1", font="Arial 15", indicatoron=False, width=5, height=2, variable=self.sliderBt1Val, state = "disabled")        
        self.sliderBt2Val = Tk.IntVar()
        self.sliderBt2 = Tk.Checkbutton(self.qtFrame, text="2", font="Arial 15", indicatoron=False, width=5, height=2, variable=self.sliderBt2Val, state = "disabled")        
        
        self.IP_label_name.grid(row=1, column=0, padx=5, pady=5, sticky="e")
        self.IP_entry_value.grid(row=1, column=1, padx=5, pady=5)
        self.Port_label_name.grid(row=2, column=0, padx=5, pady=5, sticky="e")
        self.Port_entry_value.grid(row=2, column=1, padx=5, pady=5, sticky="w")
        self.start_button.grid(row=1, column=2, rowspan=2, columnspan=1, padx=5, pady=5)
        self.qtFrame.grid(row=3, column=0,rowspan=5, columnspan=5, padx=5, pady=5)
        self.sliderWid.grid(row=5, column=1, rowspan=2, columnspan=2, padx=5, pady=5)
        self.sliderBt1.grid(row=4, column=0, rowspan=1, columnspan=1, padx=5, pady=5)
        self.sliderBt2.grid(row=5, column=0, rowspan=1, columnspan=1, padx=5, pady=5)

    def greet_func(self):
        self.start_button.config(text="Disconnect") 
        self.IP_entry_value.config(state="disabled")
        self.Port_entry_value.config(state="disabled")        

    def bye_func(self):
        self.start_button.config(text="Connect to\nServer") 
        self.start_button_val.set(0)
        self.IP_entry_value.config(state="normal")
        self.Port_entry_value.config(state="normal")        
   
    def submit_cmd(self):
        self.start_button.config(state="normal")
        self.IP_entry_value.config(state="normal")
        self.Port_entry_value.config(state="normal")
   
    def validateIP(self, new_text, full_text):
        if not full_text:
            return True
        if not new_text:
            return True
        if (new_text.isdigit() or new_text == ".") and len(full_text) < 16 and full_text.count(".") < 4:
            return True
        return False
    
    def validatePort(self, new_text, full_text):
        if not full_text:
            return True
        if not new_text:
            return True
        if new_text.isdigit() and len(full_text) < 6:
            return True
        return False
    
    def manage_start_button(self, cmd_start, cmd_stop):        
        if 1 == self.start_button_val.get():
            cmd_start()
        else:
            self.start_button.config(text="Connect to\nServer") 
            self.IP_entry_value.config(state="normal")
            self.Port_entry_value.config(state="normal")
            cmd_stop()

class ThreadedClient:
    def __init__(self, master):
        self.master = master
        self.isClientRunning = False
        self.clientSocket = None
        self.clientFrame = Tk.Frame(master, width=50, height=100, bd=1, relief="sunken")
                
        self.clientFrame.grid(padx=5, pady=5, sticky="ew")
        
        self.guiClient = ClientGui(self.clientFrame, self.clientStartCmd, self.clientStopCmd)
        
    def clientStartCmd(self):
        if (False == self.isClientRunning):
            if self.guiClient.serverIP.get() != "" and self.guiClient.serverPort.get() != "":
                pass
                self.clientThread = threading.Thread(target=self.client_thread, args = []) 
                self.clientThread.setDaemon(1)
                self.clientThread.start()            
        else:
            print("client already running")
        
    def clientStopCmd(self):
        self.isClientRunning = False        
        
    def client_thread(self):  
        host = self.guiClient.serverIP.get()
        port = int(self.guiClient.serverPort.get())
        self.guiClient.start_button.config(text="Connecting...\nClick to cancel") 
        try:
            self.clientSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.clientSocket.connect((host, port))
            self.clientSocket.settimeout(1)
            
        except socket.error as msg:
            print("Error connecting to server: " + str(msg))            
            
        else:
            self.isClientRunning = True             
            while True == self.isClientRunning:
                try:
                    data = self.clientSocket.recv(1024)
                    
                except socket.timeout:
                    continue
                    
                except socket.error as msg:
                    print('disconnected', msg)
                    break
                    
                else:
                    if len(data):
                        #print("-- before parsing --") 
                        #print(data.decode())
                        dict_str = data.decode()
                        dict_list =  [e+"}" for e in dict_str.split("}") if e]
                        for dict in dict_list:
                            parsed_data = self.parse_recv_data(dict)
                            if parsed_data is not None:
                                #print("-- received --") 
                                print(parsed_data)
                                if "data" in parsed_data:
                                    self.guiClient.greet_func()
                                elif "Button1" in parsed_data:
                                    if (parsed_data["Button1"] > 0):
                                        self.guiClient.sliderBt1Val.set(1)
                                    else:
                                        self.guiClient.sliderBt1Val.set(0)
                                elif "Button2" in parsed_data:
                                    if (parsed_data["Button2"] > 0):
                                        self.guiClient.sliderBt2Val.set(1)
                                    else:
                                        self.guiClient.sliderBt2Val.set(0)
                                elif "Slider" in parsed_data:
                                    self.guiClient.sliderVal.set(parsed_data["Slider"])
                    else:
                        print('disconnected')
                        break
                
        self.isClientRunning = False 
        self.clientSocket.close()
        print('connection lost')
        self.guiClient.bye_func()
            
    def parse_recv_data(self, rx_data):
        try:            
            convData = json.loads(rx_data)                 
        except:
            print("parsing error :invalid json data")
            return None            
        else:
            return convData

if __name__ == '__main__': 
    root = Tk.Tk()
    root.title("PIC32MZ-W1 Curiosity Touch Demo")
    ThreadedClient(root)
    root.mainloop()  