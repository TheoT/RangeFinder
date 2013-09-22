from hellousb import *
from Tkinter import *
usb=hellousb()


def moveServos(*_):
	servo1Pos=(int)((bot.get())*((2**16)-1))
	servo2Pos=(int)((top.get())*((2**16)-1))
	usb.set_vals(servo1Pos,servo2Pos)


win=Tk()
bot=DoubleVar()
top=DoubleVar()

slider1= Scale(win,variable=bot,command=moveServos,
				from_=0,to=1,resolution=.01,orient=HORIZONTAL).grid(
				row=0,column=0)

slider2= Scale(win,variable=top,command=moveServos,
				from_=0,to=1,resolution=.01,orient=HORIZONTAL).grid(
				row=1,column=0)

win.mainloop()


