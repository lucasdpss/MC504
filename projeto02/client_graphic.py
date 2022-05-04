import socket
import tkinter as tk
from PIL import ImageTk,Image

serverHost = 'localhost'
serverPort = 8080

people_queue = ['M','M','F','F','F','F','M','F','M','F']
bathroom = ['F', 'F', '']
lightswitch = 'F' #Male 'M' or Female 'F'

WIDTH = 900
HEIGHT = 540

#Janela grafica
window = tk.Tk()
window.title("Banheiro Unissex")
window.resizable(width=False, height=False)
canvas = tk.Canvas(window, width = WIDTH, height = HEIGHT)  
canvas.configure(background='black')
canvas.pack()

# Criamos o socket
cliente = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
queue = []
cliente.connect((serverHost, serverPort))
print("Conectado com o servidor.")

#imagens utilizadas
img_floor = ImageTk.PhotoImage(Image.open("assets/floor_1.png").resize((45,45)))
img_wall = ImageTk.PhotoImage(Image.open("assets/wall_mid.png").resize((45,45)))
img_door = ImageTk.PhotoImage(Image.open("assets/doors_leaf_closed.png").resize((45,45)))
img_toilet = ImageTk.PhotoImage(Image.open("assets/column_top.png").resize((45,45)))
img_female = ImageTk.PhotoImage(Image.open("assets/female.png").resize((70,60)))
img_male = ImageTk.PhotoImage(Image.open("assets/male.png").resize((70,60)))
img_lightswitch_male = ImageTk.PhotoImage(Image.open("assets/wall_banner_blue.png").resize((45,45)))
img_lightswitch_female = ImageTk.PhotoImage(Image.open("assets/wall_banner_red.png").resize((45,45)))

def draw_background():
    for i in range(int(WIDTH/45)):
        for j in range(int(HEIGHT/45)):
            canvas.create_image(i*45, j*45, anchor=tk.NW, image=img_floor)
            if(((i>=14 and i<=18) and (j==3 or j==9))or((j>=3 and j<=9) and (i==14 or i==18))):
                canvas.create_image(i*45, j*45, anchor=tk.NW, image=img_wall)
            if((i,j) in [(16,5),(17,5),(16,7),(17,7)]):
                canvas.create_image(i*45, j*45, anchor=tk.NW, image=img_wall)
            if((i,j) in [(14,6),(16,4),(16,6),(16,8)]):
                canvas.create_image(i*45, j*45, anchor=tk.NW, image=img_door)
            if((i,j) in [(17,4), (17,6),(17,8)]):
                canvas.create_image(i*45, j*45, anchor=tk.NW, image=img_toilet)

def draw_people():
    (i,j) = (13,6)
    for person in people_queue:
        if(person == 'M'):
            canvas.create_image(i*45 - 14, j*45 - 19, anchor=tk.NW, image=img_male)
        else:
            canvas.create_image(i*45 - 14, j*45 - 19, anchor=tk.NW, image=img_female)
        i -= 1
    
    (i,j) = (17,4)
    for k in range(len(bathroom)):
        if bathroom[k] == 'M':
            canvas.create_image(i*45 - 14 , (j+k*2)*45 - 19, anchor=tk.NW, image=img_male)
        elif bathroom[k] == 'F':
            canvas.create_image(i*45 - 14, (j+k*2)*45 - 19, anchor=tk.NW, image=img_female)

        

def draw_lightswitch():
    (i,j) = (14,5)
    if(lightswitch == 'F'):
        canvas.create_image(i*45, j*45, anchor=tk.NW, image=img_lightswitch_female)
        canvas.create_image(i*45, (j+2)*45, anchor=tk.NW, image=img_lightswitch_female)
    else:
        canvas.create_image(i*45, j*45, anchor=tk.NW, image=img_lightswitch_male)
        canvas.create_image(i*45, (j+2)*45, anchor=tk.NW, image=img_lightswitch_male)


try:
    while True:
        try:
            items = cliente.recv(1024)  
            data = items.decode("utf-8").split("\n")
            for i in data:
                if i != '':
                    queue.append(i)
        except socket.ConnectionResetError:
            items = 'Erro'

        if items == 'Erro' or items == b'':
            break
        
        while(len(queue) > 0):
            print(queue.pop())
            canvas.delete("all") #limpa a tela para preparar para a proxima iteracao
            draw_background()
            draw_people()
            draw_lightswitch()
            canvas.pack()
            window.update()

except KeyboardInterrupt:
    pass

cliente.close()
