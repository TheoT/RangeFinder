from hellousb import *
from Tkinter import *
import time
usb=hellousb()


def moveServos(*_):
	servo1Pos=(int)((bot.get())*((2**16)-1))
	servo2Pos=(int)((top.get())*((2**16)-1))
	usb.set_vals(servo1Pos,servo2Pos)

def ping(*_):
	usb.ping_out()
	time.sleep(0.070)
	[tof,hit]=usb.read_ping()
	print([tof,hit])
	ticks.set(str(tof))
	isHit.set(str(hit))
	if(tof==16000):
		ticks.set("N/A")
	if(tof in range(250,253)):
		ticks.set("N/A")

win=Tk()
bot=DoubleVar()
top=DoubleVar()
ticks=StringVar()
isHit=StringVar()

slider1= Scale(win,variable=bot,command=moveServos,
				from_=0,to=1,resolution=.01,orient=HORIZONTAL).grid(
				row=0,column=0)

slider2= Scale(win,variable=top,command=moveServos,
				from_=0,to=1,resolution=.01,orient=HORIZONTAL).grid(
				row=1,column=0)
pingBut= Button(win, command=ping, text="ping").grid(row=2,column=0)
label1=Label(win,text="Distance:").grid(row=3,column=1)
pingLab= Label(win, textvariable=ticks).grid(row=4,column=1)
label1=Label(win,text="Hit Detect").grid(row=3,column=0)
hitLab= Label(win, textvariable=isHit).grid(row=4,column=0)

win.mainloop()
