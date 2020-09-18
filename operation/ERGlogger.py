#!/usr/bin/python3.7

import sys
import tkinter
from tkinter import messagebox,filedialog,IntVar
from serial import Serial
from serial.tools import list_ports
from numpy import *
from scipy.interpolate import interp1d
#from matplotlib.pyplot import *
#from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg, NavigationToolbar2Tk #NavigationToolbar2TkAgg
from time import *

#+++++++++++++++++++++++++++++++
#
# DEVICE VCP COMMUNICATION CLASS
#
#+++++++++++++++++++++++++++++++
class STM:
	s = None
	def __init__(self, tty, tm):
		self.tty = tty
		self.init(tm)

	def init(self, tm):
		if self.s:
			self.s.close()
		self.s = Serial(port=self.tty, timeout=tm)

	def HandShake(self, tty):
		self.s.write('hello\r'.encode())
		rpl = self.s.readline()
		if rpl == b'':
			PRINT(' - '+tty+' did not respond!!!')
			return [0]
		rpl = self.s.readline()
		try:
			rpl = int(rpl.strip().decode())
		except:
			PRINT(' - converting to int FAILED. '+tty+' responded: '+rpl)
			return [0]

		if rpl == 167321907:
			info = []
			info.append(self.s.readline().strip().decode())
			if 'ERG-T-' in info[0]:
				for p in range(17):
					info.append(self.s.readline().strip().decode())
				# to make them real floats!!!
				info[ 6] = float(info[ 6])
				info[ 7] = float(info[ 7])
				info[ 8] = float(info[ 8])
				info[ 9] = float(info[ 9])
				info[10] = float(info[10])
				info[11] = float(info[11])
				info[12] = float(info[12])
				info[13] = float(info[13])
				info[14] = float(info[14])
				info[15] = float(info[15])
				info[16] = float(info[16])
				info[17] = float(info[17])
				PRINT(' - initial hand-shake is '+self.s.readline().strip().decode())
				return info
			elif 'ERG-TP-' in info[0]:
				for p in range(21):
					info.append(self.s.readline().strip().decode())
				# to make them real floats!!!
				info[ 7] = float(info[ 7])
				info[ 8] = float(info[ 8])
				info[ 9] = float(info[ 9])
				info[10] = float(info[10])
				info[11] = float(info[11])
				info[12] = float(info[12])
				info[13] = float(info[13])
				info[14] = float(info[14])
				info[15] = float(info[15])
				info[16] = float(info[16])
				info[17] = float(info[17])
				info[18] = float(info[18])
				info[19] = float(info[19])
				info[20] = float(info[20])
				info[21] = float(info[21])
				PRINT(' - initial hand-shake is '+self.s.readline().strip().decode())
				return info
			else:
				PRINT(' - unknown DEVICE type: '+info[0])
		else:
			PRINT(tty+' is not a proper ERG-logger device!!!')
			#PRINT(' - initial hand-shake failed: '+self.s.readline().strip().decode())
			return [0]

	def GetTime(self):
		Tx86 = time()
		while Tx86%int(Tx86) > 0.01:
			Tx86 = time()
		self.s.write(('GetTime\r'.encode()))
		rpl = self.s.readline()
		Tmcu1 = int(self.s.readline().decode())
		Tmcu2 = int(self.s.readline().decode())
		Tmcu3 = int(self.s.readline().decode())
		prscl = float(self.s.readline().decode())/10000000
		PRINT(' - date and Time information:')
		PRINT('   MCU set counter time: '+ctime(Tmcu1))
		PRINT('   MCU uncorrected time: '+ctime(Tmcu2))
		PRINT('   MCU corrected time:   '+ctime(Tmcu3))
		PRINT('   X86 reference time:   '+ctime(Tx86))
		PRINT('   difference (X86 - MCU) is %.2f per %.3e sec' % (Tx86-Tmcu2, Tmcu3-Tmcu1))
		PRINT('   current prescaler value is:     %.7f' % prscl)
		PRINT('   recommended prescaler value is: %.7f' % (1 + (Tx86 - Tmcu2)/(Tx86-Tmcu1)))
		return Tx86, Tmcu1, Tmcu2, Tmcu3, prscl

	def SetTime(self):
		self.s.write(('SetTime\r').encode())
		rpl = self.s.readline()
		rpl = self.s.readline().strip().decode()
		if rpl == 'ready':
			T = time()
			while T%int(T) > 0.001:
				T = time()
			self.s.write(int(T).to_bytes(4, byteorder='big'))
			rpl = self.s.readline().strip().decode()
			if rpl == 'OK':
				PRINT(' - time was set succesfully')
			else:
				PRINT('ERROR: Time was not set')
		else:
			PRINT('ERROR: Time was not set. LSE was not enebled. Try again!!!')

	def SetTimePrescaler(self, prscl):
		self.s.write(('SetTimePrescaler %.8f\r' % prscl).encode())
		rpl = self.s.readline()
		if self.s.readline().strip().decode() == 'OK':
			PRINT(' - the Time Prescaler was set to %.8f' % prscl)

	def GetProgramm(self):
		self.s.write(('GetProgramm\r').encode())
		rpl = self.s.readline()
		SCDL = []
		if 'ERG-T-' in DevInfo[0]:
			for p in range(4):
				SCDL.append(self.s.readline().strip().decode())
			rpl = self.s.readline().strip().decode()
		elif 'ERG-TP-' in DevInfo[0]:
			for p in range(7):
				SCDL.append(self.s.readline().strip().decode())
			rpl = self.s.readline().strip().decode()
		if rpl == 'OK':
			PRINT(' - current data collection schedule was read successfully')
			return SCDL
		else:
			PRINT(' - error in read of the current collection schedule')
			if 'ERG-T-' in DevInfo[0]:
				return ['0','0','0','0']
			elif 'ERG-TP-' in DevInfo[0]:
				return ['0','0','0','0','0','0','0']

	def SetProgramm(self, PRG):
		command = 'SetProgramm'
		for p in range(len(PRG)):
			command += ' %d' % PRG[p]
		command += '\r'
		self.s.write((command).encode())
		rpl = self.s.readline()
		rpl = self.s.readline().strip().decode()
		if rpl == 'OK':
			PRINT(' - the schedule was set successfully')
		else:
			PRINT(' - ERROR. Programm was NOT SET: '+rpl)
	def PowerOff(self):
		self.s.write('sleep\r'.encode())
		rpl = self.s.readline()
		rpl = self.s.readline()
		rpl = rpl.strip().decode()
		if rpl == 'OK':
			PRINT(' - the device was disconnected properly')
		else:
			PRINT(' - ERROR. The device was not disconected properly: '+rpl)

	# address is a three of four byte address in hexadecimal notation
	# vol - is an ammount of data to read in bytes
	# numbytes - is a number of bytes per value
	def GetData(self, fName, vol, numbytes, address = '00000000', typ = 'T'):
		# windows stuff
		MSG1 = tkinter.Toplevel(width = 270, height = 90)
		MSG1.title('Download')
		label = tkinter.Label(MSG1, text='Downloading... Please wait!', font= ('helvetica',12,'bold'))
		label.place(x=30, y = 20)
		MSG1.update_idletasks()
		tm = time()
		# stm32 communication page size that MUST NOT be a multiple of 128!!!!
		page = 64*1024
		#if 'win' in sys.platform:
		#	page = 10000    # page must be very small in windows
		#else:
		#	page = 64*1024	# 64kB in linux/MAC
		# 
		rd = 0
		RPL = b''
		while rd < vol:
			num = page*(int(rd/page) < int(vol/page)) + (vol%page)*(int(rd/page) == int(vol/page))
			self.s.write(('SendDataToX86 %d ' % num + address+'\r').encode())
			tmp = self.s.readline()
			rpl = self.s.read(num)
			#while len(rpl) != num:
			#	sleep(0.01)
			#	rpl = self.s.read(self.s.in_waiting)

			RPL += rpl
			rd += len(rpl)
			address = hex(int(address, 16) + num)
		# ACHTUNG!!! Here is an extremely important check!!!
		if vol != len(RPL):
			AUS = tkinter.messagebox.askyesno('Dowload ERROR!!!','Do you want to try to save the obtained DATA?')
			if not AUS:
				MSG1.destroy()
				return
		# conversion to the integers
		D = []
		#if not OK:
		#	vol -= 1
		for p in range(vol//numbytes):
			tmp = int.from_bytes(RPL[numbytes*p:numbytes*(p+1)], byteorder='big')
			D.append(tmp - 4294967295*(tmp > 2147483648))
		# some logical operations and a windows stuff again!
		SaveDataFile(D,fName,typ)
		dt = time()-tm
		MSG1.destroy()
		MSG2 = tkinter.messagebox.showinfo('Download','Download complete in %.1e sec!' % dt)
		#if AdwndlBox != 0:
		#	AdwndlBox.destroy()
		PRINT('   download completed succesfully in %.1e sec' % dt)


#+++++++++++++++++++++++++++++++
#
# WINDOW-SPECIFIC functions
#
#+++++++++++++++++++++++++++++++

# This one creates a list of the COM ports and ties to communicate with them.
# In case if the device responds correctly, it creates a serial STM class device
# Be careful, as the function will recognize and return the first appropriate device, which is an occasional choise!
def Device():
	success = 0
	CPlist = list_ports.comports()
	for p in range(len(CPlist)):
		dev = CPlist[p].device
		try:
			tmp = STM(tty=dev, tm=0.5)
			info = tmp.HandShake(dev)
			if info[0] != 0:
				tmp = 0
				DEV = STM(tty=dev, tm=3)
				INFO = info
				ROW = (' - '+dev+' device defined as '+info[0]+', #'+info[1]+' that is a proper ERG-logger device and is now connected')
				success = 1
		except:
			pass
	if success:
		return DEV, INFO, ROW
	else:
		ERR = tkinter.messagebox.showerror('No Device','The appropriate ERG Logger Devices were not found!\nTry to replace the USB cable :(')
		exit()

def ShowDevInfo(INFO):
	if INFO[0] == 'ERG-T-01':
		DevInfoBox = tkinter.Toplevel(width = 580, height = 230)
		DevInfoBox.title(INFO[0])
		label = tkinter.Label(DevInfoBox, text='Device MODEL: '+INFO[0], font='courier')
		label.place(x=20, y = 20)
		label = tkinter.Label(DevInfoBox, text='Devise SN:    '+INFO[1], font='courier')
		label.place(x=20, y = 40)
		label = tkinter.Label(DevInfoBox, text='MCU unit:     '+INFO[2], font='courier')
		label.place(x=20, y = 80)
		label = tkinter.Label(DevInfoBox, text='ADC unit:     '+INFO[3], font='courier')
		label.place(x=20, y = 100)
		label = tkinter.Label(DevInfoBox, text='MEMORY unit:  '+INFO[4], font='courier')
		label.place(x=20, y = 120)
		label = tkinter.Label(DevInfoBox, text='SENSOR unit:  '+INFO[5], font='courier')
		label.place(x=20, y = 140)
		CANCEL = tkinter.Button(DevInfoBox, text ="Close",  command = DevInfoBox.destroy, width = 10)
		CANCEL.place(x=220,y=190)
	elif INFO[0] == 'ERG-T-02':
		DevInfoBox = tkinter.Toplevel(width = 580, height = 230)
		DevInfoBox.title(INFO[0])
		label = tkinter.Label(DevInfoBox, text='Device MODEL: '+INFO[0], font='courier')
		label.place(x=20, y = 20)
		label = tkinter.Label(DevInfoBox, text='Devise SN:    '+INFO[1], font='courier')
		label.place(x=20, y = 40)
		label = tkinter.Label(DevInfoBox, text='MCU unit:     '+INFO[2], font='courier')
		label.place(x=20, y = 80)
		label = tkinter.Label(DevInfoBox, text='ADC unit:     '+INFO[3], font='courier')
		label.place(x=20, y = 100)
		label = tkinter.Label(DevInfoBox, text='MEMORY unit:  '+INFO[4], font='courier')
		label.place(x=20, y = 120)
		label = tkinter.Label(DevInfoBox, text='SENSOR unit:  '+INFO[5], font='courier')
		label.place(x=20, y = 140)
		CANCEL = tkinter.Button(DevInfoBox, text ="Close",  command = DevInfoBox.destroy, width = 10)
		CANCEL.place(x=220,y=190)
	if INFO[0] == 'ERG-TP-01':
		DevInfoBox = tkinter.Toplevel(width = 580, height = 250)
		DevInfoBox.title(INFO[0])
		label = tkinter.Label(DevInfoBox, text='Device MODEL:  '+INFO[0], font='courier')
		label.place(x=20, y = 20)
		label = tkinter.Label(DevInfoBox, text='Devise SN:     '+INFO[1], font='courier')
		label.place(x=20, y = 40)
		label = tkinter.Label(DevInfoBox, text='MCU unit:      '+INFO[2], font='courier')
		label.place(x=20, y = 80)
		label = tkinter.Label(DevInfoBox, text='ADC unit:      '+INFO[3], font='courier')
		label.place(x=20, y = 100)
		label = tkinter.Label(DevInfoBox, text='MEMORY unit:   '+INFO[4], font='courier')
		label.place(x=20, y = 120)
		label = tkinter.Label(DevInfoBox, text='T_SENSOR unit: '+INFO[5], font='courier')
		label.place(x=20, y = 140)
		label = tkinter.Label(DevInfoBox, text='P_SENSOR unit: '+INFO[6], font='courier')
		label.place(x=20, y = 160)
		CANCEL = tkinter.Button(DevInfoBox, text ="Close",  command = DevInfoBox.destroy, width = 10)
		CANCEL.place(x=220,y=210)

def ShowCalibration(INFO):
	if 'ERG-T-' in INFO[0]:
		CalibrBox = tkinter.Toplevel(width = 650, height = 260)
		CalibrBox.title(INFO[0])
		label = tkinter.Label(CalibrBox, text='A10 = %11.4e;' % float(INFO[6]), font='courier')
		label.place(x=20, y = 20)
		label = tkinter.Label(CalibrBox, text='A11 = %11.4e;' % float(INFO[7]), font='courier')
		label.place(x=20, y = 40)
		label = tkinter.Label(CalibrBox, text='A12 = %11.4e;' % float(INFO[8]), font='courier')
		label.place(x=20, y = 60)
		label = tkinter.Label(CalibrBox, text=' R1 = %11.5f;' % float(INFO[9]), font='courier')
		label.place(x=20, y = 80)
		label = tkinter.Label(CalibrBox, text='A20 = %11.4e;' % float(INFO[10]), font='courier')
		label.place(x=230, y = 20)
		label = tkinter.Label(CalibrBox, text='A21 = %11.4e;' % float(INFO[11]), font='courier')
		label.place(x=230, y = 40)
		label = tkinter.Label(CalibrBox, text='A22 = %11.4e;' % float(INFO[12]), font='courier')
		label.place(x=230, y = 60)
		label = tkinter.Label(CalibrBox, text=' R2 = %11.5f;' % float(INFO[13]), font='courier')
		label.place(x=230, y = 80)
		label = tkinter.Label(CalibrBox, text='A30 = %11.4e;' % float(INFO[14]), font='courier')
		label.place(x=440, y = 20)
		label = tkinter.Label(CalibrBox, text='A31 = %11.4e;' % float(INFO[15]), font='courier')
		label.place(x=440, y = 40)
		label = tkinter.Label(CalibrBox, text='A32 = %11.4e;' % float(INFO[16]), font='courier')
		label.place(x=440, y = 60)
		label = tkinter.Label(CalibrBox, text=' R3 = %11.5f;' % float(INFO[17]), font='courier')
		label.place(x=440, y = 80)
		CANCEL = tkinter.Button(CalibrBox, text ="Close",  command = CalibrBox.destroy, width = 10)
		CANCEL.place(x=500,y=130)
	if 'ERG-TP-' in INFO[0]:
		CalibrBox = tkinter.Toplevel(width = 650, height = 270)
		CalibrBox.title(INFO[0])
		label = tkinter.Label(CalibrBox, text='Temperature coefficients:', font='courier')
		label.place(x=20, y = 20)
		label = tkinter.Label(CalibrBox, text='A10 = %11.4e;' % float(INFO[7]), font='courier')
		label.place(x=20, y = 40)
		label = tkinter.Label(CalibrBox, text='A11 = %11.4e;' % float(INFO[8]), font='courier')
		label.place(x=20, y = 60)
		label = tkinter.Label(CalibrBox, text='A12 = %11.4e;' % float(INFO[9]), font='courier')
		label.place(x=20, y = 80)
		label = tkinter.Label(CalibrBox, text=' R1 = %11.5f;' % float(INFO[10]), font='courier')
		label.place(x=20, y = 100)
		label = tkinter.Label(CalibrBox, text='A20 = %11.4e;' % float(INFO[11]), font='courier')
		label.place(x=230, y = 40)
		label = tkinter.Label(CalibrBox, text='A21 = %11.4e;' % float(INFO[12]), font='courier')
		label.place(x=230, y = 60)
		label = tkinter.Label(CalibrBox, text='A22 = %11.4e;' % float(INFO[13]), font='courier')
		label.place(x=230, y = 80)
		label = tkinter.Label(CalibrBox, text=' R2 = %11.5f;' % float(INFO[14]), font='courier')
		label.place(x=230, y = 100)
		label = tkinter.Label(CalibrBox, text='A30 = %11.4e;' % float(INFO[15]), font='courier')
		label.place(x=440, y = 40)
		label = tkinter.Label(CalibrBox, text='A31 = %11.4e;' % float(INFO[16]), font='courier')
		label.place(x=440, y = 60)
		label = tkinter.Label(CalibrBox, text='A32 = %11.4e;' % float(INFO[17]), font='courier')
		label.place(x=440, y = 80)
		label = tkinter.Label(CalibrBox, text=' R3 = %11.5f;' % float(INFO[18]), font='courier')
		label.place(x=440, y = 100)
		label = tkinter.Label(CalibrBox, text='Pressure coefficients:', font='courier')
		label.place(x=20, y = 140)
		label = tkinter.Label(CalibrBox, text='P1 = %10.4e;' % float(INFO[19]), font='courier')
		label.place(x=20, y = 160)
		label = tkinter.Label(CalibrBox, text='P2 = %10.4e;' % float(INFO[20]), font='courier')
		label.place(x=230, y = 160)
		label = tkinter.Label(CalibrBox, text='P3 = %10.4e;' % float(INFO[21]), font='courier')
		label.place(x=440, y = 160)
		CANCEL = tkinter.Button(CalibrBox, text ="Close",  command = CalibrBox.destroy, width = 10)
		CANCEL.place(x=500,y=210)

def ShowTime():
	T = stm32.GetTime()
	TimeBox = tkinter.Toplevel(width = 620, height = 290)
	TimeBox.title('Time Settings')
	label = tkinter.Label(TimeBox, text='Current X86 date is '+ctime(T[0]), font='courier')
	label.place(x=20, y = 20)
	label = tkinter.Label(TimeBox, text='Current MCU date is '+ctime(T[3]), font='courier')
	label.place(x=20, y = 40)
	label = tkinter.Label(TimeBox, text='Date was reseted at '+ctime(T[1]), font='courier')
	label.place(x=20, y = 60)
	label = tkinter.Label(TimeBox, text='Whould you like to set the Date and Time?', font='courier')
	label.place(x=20, y = 90)
	SetTimeY = tkinter.Button(TimeBox, text ="Yes", command = SetTime, width=8)
	SetTimeY.place(x=60,y=120)
	label = tkinter.Label(TimeBox, text='Current Time Prescaler value is %9.7f' %T[4], font='courier')
	label.place(x=20, y = 170)
	label = tkinter.Label(TimeBox, text=' Proper Time Prescaler value is %9.7f' % (1 + (T[0]-T[2])/(T[0]-T[1])), font='courier')
	label.place(x=20, y = 190)
	label = tkinter.Label(TimeBox, text='Whould you like to set a new Time Prescaler value?', font='courier')
	label.place(x=20, y = 220)
	SetPrsclY = tkinter.Button(TimeBox, text ="Yes", command = lambda: SetTimePrescaler(1 + (T[0] - T[2])/(T[0]-T[1])), width=8)
	SetPrsclY.place(x=60,y=250)
	CANCEL = tkinter.Button(TimeBox, text ="Cancel",  command = TimeBox.destroy, width = 8)
	CANCEL.place(x=410,y=250)

def SetTime():
	AUS = tkinter.messagebox.askyesno('Set Date and Time', 'Are You Sure???')
	if AUS:
		stm32.SetTime()

def SetTimePrescaler(prscl):
	AUS = tkinter.messagebox.askyesno('Set Time Prescaler', 'The Prescaler MUST NOT be set, if the Device has not worked continuously at least one month since last Date and Time reset\n\nSet the Prescaler???')
	if AUS:
		stm32.SetTimePrescaler(prscl)

def SetSchedule():
	NST = mktime(struct_time([int(syear.get()),int(smnth.get()),int(sday.get()),int(shour.get()),int(smin.get()),int(ssec.get()),0,0,0]))
	NFN = mktime(struct_time([int(fyear.get()),int(fmnth.get()),int(fday.get()),int(fhour.get()),int(fmin.get()),int(fsec.get()),0,0,0]))
	# IMPORTANT NOTE!!!! REMOVE THE MUTLIPLIER 60 TO FORCE THE DEVICE TO MAKE MEASUREMENTS IN THE SECONDS SCALE
	if 'ERG-T-' in DevInfo[0]:
		#FRQ = 60*freqT.get()
		TFRQ = int(freqT.get())
		if TFRQ != float(TFRQ):
			RJ = tkinter.messagebox.showerror('Frequency ERROR','Please, provide an integer value for the DATA ACQUISITION frequency!')
			return
	elif 'ERG-TP-' in DevInfo[0]:
		#TFRQ = 60*freqT.get()
		#PFRQ = 60*freqP.get()
		TFRQ = int(freqT.get())
		PFRQ = int(freqP.get())
		if TFRQ != float(TFRQ):
			RJ = tkinter.messagebox.showerror('Frequency ERROR','Please, provide an integer value for the TEMPERATURE ACQUISITION frequency!')
			return
		if PFRQ != float(PFRQ):
			RJ = tkinter.messagebox.showerror('Frequency ERROR','Please, provide an integer value for the PRESSURE ACQUISITION frequency!')
			return
		# initial address for the pressure storage definition
		Paddr = GetPaddr(int(TFRQ),int(PFRQ));
	# number of points
	numT = (NFN - NST)//TFRQ
	if 'ERG-TP-' in DevInfo[0]:
		numP = (NFN - NST)//PFRQ
	# the sFLASH memory available
	NUM = GetFlashSize()
	# schedule length and memmory check
	if 'ERG-T-' in DevInfo[0]:
		if NUM < 16*numT:
			RJ = tkinter.messagebox.showerror('MEMORY ERROR','You have requested %dMB of the memory, while there is available only %dMB.' % (16*numT/(1024**2),NUM/(1024**2)))
			PRINT(' - the data collection schedule WAS NOT set due to the MEMORY restrictions')
			return

	elif 'ERG-TP-' in DevInfo[0]:
		if NUM < (16*numT+8*numP):
			RJ = tkinter.messagebox.showerror('MEMORY ERROR','You have requested %dMB of the memory for the TEMPERATURE and %dMB for the PRESSURE, while there is available only %dMB.' % (16*numT/(1024**2),8*numP/(1024**2),NUM/(1024**2)))
			PRINT(' - the data collection schedule WAS NOT set due to the MEMORY restrictions')
			return

	# START-STOP checking
	# check if the START date is placed before the finish date
	if NST > NFN:
		RJ = tkinter.messagebox.showerror('schedule ERROR','The data collection START date must be before FINISH date!!!')
	else:
		# checks the dates if they are in the past and decides what to say to the user
		tm = time()
		if (NST < tm) and (NFN > tm):
			AUS = tkinter.messagebox.askyesno('ACHTUNG','The date of the data collection START is in the PAST.\nThe schedule will be shifted forward.\n\nProceed?')
			if AUS:
				# check the length of the schedule
				if (NFN - tm - 30) > (2*365*24*3600):
					RJ = tkinter.messagebox.showwarning('Schedule WARNING','You are requesting for the schedule longer than two years. It could be too long for the battery cells of the device!')

				MSG = 'The DATA collection will start 10 min after the USB cable disconnection\n'+\
				      'The estimated start date is\n'+ctime(tm+10)+'\n'+\
				      'The estimated finish date is\n'+ctime(tm+10 + int(FRQ)*num*60)+'\n'+\
				      'The total programm length is %.f days' % ((NFN - NST - 30)/86400)+'\n'+\
				      'There will be collected %d DATA points\n' % num + '\n\n'+\
				      'Set the schedule?'
				NFN = NFN + (tm - NST)
				NST = tm
			else:
				PRINT(' - the data collection schedule WAS NOT set due to the wrong START date')
				return
		elif NFN < tm:
			MSG = 'ACHTUNG!!!\nThe date of data collection FINISH is in the PAST.\nNo DATA will be collected!!!'
		else:
			# check the length of the schedule
			if (NFN - NST) > (2*365*24*3600):
				RJ = tkinter.messagebox.showwarning('Schedule WARNING','You are requesting a schedule longer than two years. It could be too long for the battery cells of the device!')
			if 'ERG-T-' in DevInfo[0]:
				MSG = 'The DATA collection will start at\n'+ctime(NST)+'\n'+\
				      'The DATA collection will finish at\n'+ctime(NFN)+'\n'+\
				      'The DATA collection frequency is %d sec\n' % int(TFRQ) +\
				      'The total programm length is %.f days' % ((NFN - NST)/86400)+'\n'+\
				      'There will be collected %d DATA points\n' % numT + '\n\n'+\
				      'Set the schedule?'
			elif 'ERG-TP-' in DevInfo[0]:
				MSG = 'The DATA collection will  start at\n'+ctime(NST)+'\n'+\
				      'The DATA collection will finish at\n'+ctime(NFN)+'\n'+\
				      'The DATA collection frequency is\n' +\
				      '%3d sec for TEMPERATURE and\n' % int(TFRQ) +\
				      '%3d sec for PRESSURE\n' % int(PFRQ) +\
				      'The schedule length is %.f days\n' % ((NFN - NST)/86400) +\
				      'There will be collected:\n' +\
				      '%d TEMPERATURE DATA points\n' % numT +\
				      '%d    PRESSURE DATA points\n' % numP + '\n\n'+\
				      'Set the schedule?'


		# schedule setting and log messages production
		AUS = tkinter.messagebox.askyesno('Schedule Settings', MSG)
		if AUS:
			PRINT(' - the DATA collection schedule settings:')
			MSG = MSG.split('\n')
			for p in range(len(MSG)-1):
				if MSG[p] != '':
					PRINT('   '+MSG[p])

			if 'ERG-T-' in DevInfo[0]:
				stm32.SetProgramm([int(TFRQ),NST,NFN])
			elif 'ERG-TP-' in DevInfo[0]:
				stm32.SetProgramm([int(TFRQ),int(PFRQ),Paddr,NST,NFN])

			# TODO !!! It must reread the schedule and show it to the user!!!
		else:
			PRINT(' - the DATA collection schedule WAS NOT set due to the USER abort')

def DataDownload(mode):
	# file names definition
	global fNameT, fNameP
	if 'ERG-T-' in DevInfo[0]:
		fnameT = (DevInfo[0]+'-'+DevInfo[1]+'-'+str(TM.tm_year)+'.%2d.%2d-%2d.%2d.log' % (TM.tm_mon, TM.tm_mday, TM.tm_hour, TM.tm_min)).replace(' ','0')
		fNameT = filedialog.asksaveasfilename(title='Choose a file', initialfile = fnameT)
		if (fNameT != ()) and (fNameT != ''):
			PRINT(' - filename choosen: '+fNameT)
		else:
			return
	elif 'ERG-TP-' in DevInfo[0]:
		if Pdwnl.get() == 1:
			fnameP = (DevInfo[0]+'-'+DevInfo[1]+'-PRESSURE-'+str(TM.tm_year)+'.%2d.%2d-%2d.%2d.log' % (TM.tm_mon, TM.tm_mday, TM.tm_hour, TM.tm_min)).replace(' ','0')
			fNameP = filedialog.asksaveasfilename(title='Choose a PRESSURE data file name', initialfile = fnameP)
			fNameT = 'ERG-T.tmp'
			if (fNameP != ()) and (fNameP != ''):
				PRINT(' - filename choosen: '+fNameP)
			else:
				return
		if Tdwnl.get() == 1:
			fnameT = (DevInfo[0]+'-'+DevInfo[1]+'-TEMPERATURE-'+str(TM.tm_year)+'.%2d.%2d-%2d.%2d.log' % (TM.tm_mon, TM.tm_mday, TM.tm_hour, TM.tm_min)).replace(' ','0')
			fNameT = filedialog.asksaveasfilename(title='Choose a TEMPERATURE data file name', initialfile = fnameT)
			if (fNameT != ()) and (fNameT != ''):
				PRINT(' - filename choosen: '+fNameT)
			else:
				return
		if (Tdwnl.get() == 0) and (Pdwnl.get() == 0):
				ERR = tkinter.messagebox.showerror('Download ERROR','Please, choose the data you whould like to download!')
				return
	else:
				ERR = tkinter.messagebox.showerror('Download ERROR','Unknown DEVICE type: '+DevInfo[0])
				return
	#DATA = [0]
	# ADVANCED MODE
	if mode == 'A':
		if 'ERG-T-' in DevInfo[0]:
			#global AdwndlBox
			MSG =  'The ADVANCED DOWNLOAD mode allows to download\n'
			MSG += 'all the DATA from the internal memory.\n\n'
			MSG += 'User shoud provide the beginning adress,\n'
			MSG += 'or the number of the data samples do download.\n'
			MSG += 'The address is a HEXADECIMAL 24 bit value:\n'
			MSG += 'Defauls is zero, and final is FFFFFF.\n'
			MSG += 'The starting address MUST be a multiple of 16!!!\n'
			MSG += 'The amount of the data samples is limited to 1048576.\n'
			AdwndlBox = tkinter.Toplevel(width = 580, height = 300)
			AdwndlBox.title('Advanced Download')
			label = tkinter.Label(AdwndlBox, text=MSG, font='courier', justify='left')
			label.place(x=20, y=20)
			label = tkinter.Label(AdwndlBox, text='ADDRESS:', font='courier')
			label.place(x=20, y=210)
			addr = tkinter.Spinbox(AdwndlBox, width = 10, justify='center', textvariable = tkinter.IntVar(0))
			addr.place(x=100, y=212)
			label = tkinter.Label(AdwndlBox, text='N of SAMPLES:', font='courier')
			label.place(x=240, y=210)
			amnt = tkinter.Spinbox(AdwndlBox, width = 10, justify='center', textvariable = tkinter.IntVar(0))
			amnt.place(x=370, y=212)
			DwnldB = tkinter.Button(AdwndlBox, text ="Download", command = lambda: stm32.GetData(fNameT, int(amnt.get())*16, 4, address = addr.get().replace(' ','0')), width = 25)
			DwnldB.place(x=40,y=250)
			DwnldB = tkinter.Button(AdwndlBox, text ="Close", command = AdwndlBox.destroy, width = 25)
			DwnldB.place(x=300,y=250)
		if 'ERG-TP-' in DevInfo[0]:
			#global AdwndlBox
			MSG =  'The ADVANCED DOWNLOAD mode allows to download all the DATA from the internal memory.\n\n'
			MSG += 'User shoud provide the number of data points to download for temperature and/or pressure \n'
			MSG += 'Alternatively, user may provide the start and fish address, do download.\n'
			MSG += 'The address is a HEXADECIMAL 24 bit value. Initial is zero, and final is FFFFFF.\n'
			MSG += 'The starting address MUST be a multiple of 16!!!\n'
			AdwndlBox = tkinter.Toplevel(width = 920, height = 260)
			AdwndlBox.title('Advanced Download')
			label = tkinter.Label(AdwndlBox, text=MSG, font='courier', justify='left')
			label.place(x=20, y=20)
			if Pdwnl.get() == 1:
				label = tkinter.Label(AdwndlBox, text='N of P-SAMPLES:', font='courier')
				label.place(x=320, y=150)
				amntP = tkinter.Spinbox(AdwndlBox, width = 10, justify='center', textvariable = tkinter.IntVar(0))
				amntP.place(x=500, y=150)
				PDwnldB = tkinter.Button(AdwndlBox, text ="Download", command = lambda: stm32.GetData(fNameP, int(amntP.get())*16, numbytes = 4, address = hex(InitAddrP), typ = 'P'), width = 25)
				PDwnldB.place(x=320,y=180)
			if (Tdwnl.get() == 1) or (Pdwnl.get() == 1):
				label = tkinter.Label(AdwndlBox, text='N of T-SAMPLES:', font='courier')
				label.place(x=20, y=150)
				amntT = tkinter.Spinbox(AdwndlBox, width = 10, justify='center', textvariable = tkinter.IntVar(0))
				amntT.place(x=200, y=150)
				TDwnldB = tkinter.Button(AdwndlBox, text ="Download", command = lambda: stm32.GetData(fNameT, int(amntT.get())*16, numbytes = 4, address = '00000000'), width = 25)
				TDwnldB.place(x=20,y=180)
			#
			#label = tkinter.Label(AdwndlBox, text='ADDRESS START:', font='courier')
			#label.place(x=20, y=210)
			#addr = tkinter.Spinbox(AdwndlBox, width = 10, justify='center', textvariable = tkinter.IntVar(0))
			#addr.place(x=100, y=212)
			#label = tkinter.Label(AdwndlBox, text='FINISH:', font='courier')
			#label.place(x=20, y=210)
			#addr = tkinter.Spinbox(AdwndlBox, width = 10, justify='center', textvariable = tkinter.IntVar(0))
			#addr.place(x=100, y=212)
			DwnldB = tkinter.Button(AdwndlBox, text ="Close", command = AdwndlBox.destroy, width = 25)
			DwnldB.place(x=320,y=220)
	# BASIC MODE
	elif mode == 'B':
		if 'ERG-T-' in DevInfo[0]:
			vol = int(OSCD[0], 16)
			if vol == 0:
				RJ = tkinter.messagebox.showerror('Download ERROR','There is no new data in the memory.\nTry the ADVANCED DOWNLOAD')
				return
			else:
				stm32.GetData(fNameT, vol, numbytes = 4)
		elif 'ERG-TP-' in DevInfo[0]:
			if (Tdwnl.get() == 1) or (Pdwnl.get() == 1):
				vol = int(OSCD[0], 16)
				if vol == 0:
					RJ = tkinter.messagebox.showerror('Download ERROR','There is no new TEMPERATURE data in the memory.\nTry the ADVANCED DOWNLOAD')
				else:
					stm32.GetData(fNameT, vol, numbytes = 4)

			if Pdwnl.get() == 1:
				vol = int(OSCD[1],16) - InitAddrP
				if vol <= 0:
					RJ = tkinter.messagebox.showerror('Download ERROR','There is no new PRESSURE data in the memory.\nTry the ADVANCED DOWNLOAD')
					return
				else:
					stm32.GetData(fNameP, vol, numbytes = 4, address=OSCD[2], typ = 'P')
def SaveDataFile(DATA,fName,typ):
	if typ == 'T':
		tm = array(DATA[0::4])
		V1 = array(DATA[1::4])
		V2 = array(DATA[2::4])
		V3 = array(DATA[3::4])
		if 'ERG-T-' in DevInfo[0]:
			T1 = Tenz2Temp(V1,  DevInfo[6],  DevInfo[7],  DevInfo[8],  DevInfo[9])
			T2 = Tenz2Temp(V2, DevInfo[10], DevInfo[11], DevInfo[12], DevInfo[13])
			T3 = Tenz2Temp(V3, DevInfo[14], DevInfo[15], DevInfo[16], DevInfo[17])
		elif 'ERG-TP-' in DevInfo[0]:
			T1 = Tenz2Temp(V1,  DevInfo[7],  DevInfo[8],  DevInfo[9], DevInfo[10])
			T2 = Tenz2Temp(V2, DevInfo[11], DevInfo[12], DevInfo[13], DevInfo[14])
			T3 = Tenz2Temp(V3, DevInfo[15], DevInfo[16], DevInfo[17], DevInfo[18])
		W = open(fName, 'w')
		W.write('# Temperature log-file created '+ctime()+' by ERGlogger.py\n# Device model: '+DevInfo[0]+', Device Serial Number: '+DevInfo[1]+'\n#\t\t\t\t\t\tDr. Zoidberg\n#\n')
		for p in range(len(tm)):
			#W.write(ctime(tm[p])+' '+ctime(T1[p])+' '+ctime(T2[p])+'\n')
			W.write(ctime(tm[p])+'\t%8.4f\t%8.4f\t%8.4f\n' % (T1[p],T2[p],T3[p]))
		W.close()
		W = open(fName.replace(fName.split('/')[-1],'VOLTAGE-'+fName.split('/')[-1]), 'w')
		W.write('# Voltage (in nV) log-file created '+ctime()+' by ERGlogger.py\n# Device model: '+DevInfo[0]+', Device Serial Number: '+DevInfo[1]+'\n#\t\t\t\t\t\tDr. Zoidberg\n#\n')
		for p in range(len(tm)):
			W.write(ctime(tm[p])+'\t%9d\t%9d\t%9d\n' % (V1[p],V2[p],V3[p]))
		W.close()
	elif typ == 'P':
		tm = array(DATA[0::2])
		V  = array(DATA[1::2])
		P  = Tenz2Press(tm, V, DevInfo[19], DevInfo[20], DevInfo[21])
		tm = tm[:len(P)]
		W = open(fName, 'w')
		W.write('# Pressure (Bar) log-file created '+ctime()+' by ERGlogger.py\n# Device model: '+DevInfo[0]+', Device Serial Number: '+DevInfo[1]+'\n#\t\t\t\t\t\tDr. Zoidberg\n#\n')
		for p in range(len(tm)):
			W.write(ctime(tm[p])+'\t%8.5f\n' % P[p])
		W.close()
		W = open(fName.replace(fName.split('/')[-1],'VOLTAGE-'+fName.split('/')[-1]), 'w')
		W.write('# Voltage (in nV) log-file created '+ctime()+' by ERGlogger.py\n# Device model: '+DevInfo[0]+', Device Serial Number: '+DevInfo[1]+'\n#\t\t\t\t\t\tDr. Zoidberg\n#\n')
		for p in range(len(tm)):
			W.write(ctime(tm[p])+'\t%9d\n' % V[p])
		W.close()

def Tenz2Temp(U,A0,A1,A2,R0):
       	V  =  1250.0   # mV
        R1 =  1050.0   # Ohm
        R2 =  5000.0   # Ohm
        U = U/1000000  # to convert it from nV to mV
        T = ((R2/R0)*(V*R1 + U*(R1+R2))/(V*R2 - U*(R1+R2)) - 1)/A0
        R1 =  R1*(1 + T*A1)
        R2 =  R2*(1 + T*A2)
        return ((R2/R0)*(V*R1 + U*(R1+R2))/(V*R2 - U*(R1+R2)) - 1)/A0

#def Tenz2Temp(U,A,R0):
#	V  =  1250.0   # mV
#	R1 =  1050.0   # Ohm
#	R2 =  5000.0   # Ohm
#	U = U/1000000  # to convert it from nV to mV
#	#return (1/A)*( U*R1*(R1+R2)*(1/R0+1/R1) - V*(R1/R0)*(R0-R2))/(V*R1 - U*(R1 + R2))
#	return ((R2/R0)*(V*R1 + U*(R1+R2))/(V*R2 - U*(R1+R2)) - 1)/A

def Tenz2Press(tm, U, A1, A2, A3):
	global fNameT
	T = readlog(fNameT,'T')
	TT = interp1d(T[:,0],T[:,1:].mean(1))
	UU = interp1d(tm, U)
	tm = tm[:-int((T[1,0]-T[0,0])/(tm[1]-tm[0]))-1]
	# temperature drift removal
	U = UU(tm) + A1*TT(tm)
	# linear conversion
	P = A2*U + A3
	return P/1000

def PRINT(row):
	print(row)
	oFile.write(row+'\n')
	oFile.flush()

def QUIT():
	try:
		stm32.PowerOff()
	except:
		PRINT(' - POWER was not switched off properly!!!')
	exit()

# it splits the flash exactly proportional to the memory required for TEMP and PRESSURE storage
def GetPaddr(TFRQ,PFRQ):
	NUM = GetFlashSize()
	if (TFRQ == 0) or (PFRQ == 0):
		return 0
	# equal frequency: 11/5 = T/P
	else:
		addr = int(NUM*int(1024*(16/TFRQ)/(16/TFRQ+8/PFRQ))/1024)
		return addr

def GetFlashSize():
	if 'MX25L64' in DevInfo[4]:
		return int(64*1024**2/8)
	elif 'MX25L128' in DevInfo[4]:
		return int(128*1024**2/8)
	elif 'MX25L256' in DevInfo[4]:
		return int(256*1024**2/8)
	else:
		RJ = tkinter.messagebox.showerror('MEMORY ERROR','The type of FLASH was not recognized. Sorry!!!')
		PRINT(' - the DATA collection schedule WAS NOT set due to the FLASH recognition problem')
		return 0

# reads the ERG log files and returns the array with the time in epoch notation
def readlog(name, mode):
	D = []
	F = open(name, 'r')
	line = F.readline()
	while line[0] == '#':
		line = F.readline()
	while line != '':
		tm = mktime(strptime(line[:24]))
		line = line.split()
		if mode == 'T':
			D.append([tm, float(line[5]),float(line[6]),float(line[7])])
		elif mode == 'P':
			D.append([tm, float(line[5])])
		line = F.readline()
	return array(D)

#+++++++++++++++++++++++++++++++
#
# THE MAIN BODY OF THE :)
#
#+++++++++++++++++++++++++++++++


# the OPERATION log-file!
TM = strptime(ctime())
oFile = open(('OPERATION-'+str(TM.tm_year)+'.%2d.%2d-%2d.%2d.log' % (TM.tm_mon, TM.tm_mday, TM.tm_hour, TM.tm_min)).replace(' ','0'),'w')
#oFile = open(('OPERATION-TEST.log'),'w')
# VC and STM class creation
stm32, DevInfo, row = Device()
if stm32 == 0:
	exit()
PRINT(row)
fNameP = 'q'
fNameT = 'q'
# old schedule acquisition
OSCD = stm32.GetProgramm()    # Old Schedule: CurrentLastAddress, Start, Stop, Frequency
if 'ERG-T-' in DevInfo[0]:
	OST  = strptime(ctime(int(OSCD[1]))) # Old Shedule Start Point
	OFN  = strptime(ctime(int(OSCD[2]))) # Old Shedule Start Point
	PRINT('   final address is: '+OSCD[0])
	PRINT('   there should be %d points in the log-file' % (int(OSCD[0],16)//16))
	PRINT('   old   frequency:  %d'  % (int(OSCD[3])))
	PRINT('   old  START date:  '+ctime(int(OSCD[1])))
	PRINT('   old FINISH date:  '+ctime(int(OSCD[2])))
if 'ERG-TP-' in DevInfo[0]:
	OST  = strptime(ctime(int(OSCD[3]))) # Old Shedule Start Point
	OFN  = strptime(ctime(int(OSCD[4]))) # Old Shedule Start Point
	InitAddrP = int(OSCD[2],16)
	PRINT('   final TEMPERATURE address is: %6x' % int(OSCD[0],16))
	PRINT('   final    PRESSURE address is: %6x' % int(OSCD[1],16))
	PRINT('   there should be %6d TEMPERATURE points in the log-file' % (int(OSCD[0],16)//16))
	if int(OSCD[1],16) >= InitAddrP:
		PRINT('   there should be %6d    PRESSURE points in the log-file' % ((int(OSCD[1],16) - InitAddrP)//8))
	else:
		PRINT('   there should be %6d    PRESSURE points in the log-file' % 0)
	PRINT('   old TEMPERATURE frequency:  %d' % (int(OSCD[5])))
	PRINT('   old    PRESSURE frequency:  %d' % (int(OSCD[6])))
	PRINT('   old  START date:  '+ctime(int(OSCD[3])))
	PRINT('   old FINISH date:  '+ctime(int(OSCD[4])))

# ROOT program window definitions
if 'ERG-T-' in DevInfo[0]:
	root = tkinter.Tk()
	root.title('ERG-LOGGER, v 1.01')
	root.protocol('WM_DELETE_WINDOW', QUIT)
	root.geometry('570x700')
	# device information
	label = tkinter.Label(root, text='The device '+DevInfo[0]+' (SN: '+DevInfo[1]+') is ON-LINE', font='courier')
	label.place(x=20, y=20)
	DevInfoB = tkinter.Button(root, text ="Device Information", command = lambda: ShowDevInfo(DevInfo), width = 25)
	DevInfoB.place(x=50,y=50)
	DevInfoB = tkinter.Button(root, text ="Calibration Coefficients", command = lambda: ShowCalibration(DevInfo), width = 25)
	DevInfoB.place(x=300,y=50)
	# time
	label = tkinter.Label(root, text='Date and Time settings and information:', font='courier')
	label.place(x=20, y=100)
	TimeInfoB = tkinter.Button(root, text ="Date & Time Settings", command = ShowTime, width = 25)
	TimeInfoB.place(x=50,y=130)
	# programm
	label = tkinter.Label(root, text='data collection schedule:', font='courier')
	label.place(x=20, y=180)
	label = tkinter.Label(root, text='collection frequency (every X seconds):', font='courier')
	label.place(x=40, y=210)
	freqT = tkinter.Spinbox(root, from_ = 1, to = 86400, width = 5, justify='center', textvariable = tkinter.IntVar(value=int(OSCD[3])))
	freqT.place(x=460, y=212)
	label = tkinter.Label(root, text='data collection START date and time', font='courier')
	label.place(x=40, y=240)
	label = tkinter.Label(root, text='day:', font='courier')
	label.place(x=60, y=270)
	sday = tkinter.Spinbox(root, from_ = 1, to = 31, width = 5, justify='center', textvariable = tkinter.IntVar(value=OST.tm_mday))
	sday.place(x=100, y=272)
	label = tkinter.Label(root, text='month:', font='courier')
	label.place(x=170, y=270)
	smnth = tkinter.Spinbox(root, from_ = 1, to = 12, width = 5, justify='center', textvariable = tkinter.IntVar(value=OST.tm_mon))
	smnth.place(x=230, y=272)
	label = tkinter.Label(root, text='year:', font='courier')
	label.place(x=310, y=270)
	syear = tkinter.Spinbox(root, from_ = 1900, to = 3000, width = 5, justify='center', textvariable = tkinter.IntVar(value=OST.tm_year))
	syear.place(x=360, y=272)
	label = tkinter.Label(root, text='hour:', font='courier')
	label.place(x=50, y=300)
	shour = tkinter.Spinbox(root, from_ = 0, to = 24, width = 5, justify='center', textvariable = tkinter.IntVar(value=OST.tm_hour))
	shour.place(x=100, y=302)
	label = tkinter.Label(root, text='min:', font='courier')
	label.place(x=190, y=300)
	smin = tkinter.Spinbox(root, from_ = 0, to = 60, width = 5, justify='center', textvariable = tkinter.IntVar(value=OST.tm_min))
	smin.place(x=230, y=302)
	label = tkinter.Label(root, text='sec:', font='courier')
	label.place(x=320, y=300)
	ssec = tkinter.Spinbox(root, from_ = 0, to = 60, width = 5, justify='center', textvariable = tkinter.IntVar(value=OST.tm_sec))
	ssec.place(x=360, y=302)

	label = tkinter.Label(root, text='data collection FINISH date and time', font='courier')
	label.place(x=40, y=340)
	label = tkinter.Label(root, text='day:', font='courier')
	label.place(x=60, y=370)
	fday = tkinter.Spinbox(root, from_ = 1, to = 31, width = 5, justify='center', textvariable = tkinter.IntVar(value=OFN.tm_mday))
	fday.place(x=100, y=372)
	label = tkinter.Label(root, text='month:', font='courier')
	label.place(x=170, y=370)
	fmnth = tkinter.Spinbox(root, from_ = 1, to = 12, width = 5, justify='center', textvariable = tkinter.IntVar(value=OFN.tm_mon))
	fmnth.place(x=230, y=372)
	label = tkinter.Label(root, text='year:', font='courier')
	label.place(x=310, y=370)
	fyear = tkinter.Spinbox(root, from_ = 1900, to = 3000, width = 5, justify='center', textvariable = tkinter.IntVar(value=OFN.tm_year))
	fyear.place(x=360, y=372)
	label = tkinter.Label(root, text='hour:', font='courier')
	label.place(x=50, y=400)
	fhour = tkinter.Spinbox(root, from_ = 0, to = 24, width = 5, justify='center', textvariable = tkinter.IntVar(value=OFN.tm_hour))
	fhour.place(x=100, y=402)
	label = tkinter.Label(root, text='min:', font='courier')
	label.place(x=190, y=400)
	fmin = tkinter.Spinbox(root, from_ = 0, to = 60, width = 5, justify='center', textvariable = tkinter.IntVar(value=OFN.tm_min))
	fmin.place(x=230, y=402)
	label = tkinter.Label(root, text='sec:', font='courier')
	label.place(x=320, y=400)
	fsec = tkinter.Spinbox(root, from_ = 0, to = 60, width = 5, justify='center', textvariable = tkinter.IntVar(value=OFN.tm_sec))
	fsec.place(x=360, y=402)

	MSG =	'ACHTUNG!!!\n'+\
		'User MUST download the DATA before setting\n'+\
		'the SCHEDULE. Otherwise the DATA can be downloaded\n'+\
		'only in the ADVANCED DOWNLOAD mode'

	label = tkinter.Label(root, text=MSG, font='courier',justify='left')
	label.place(x=40, y=440)
	SetPrB = tkinter.Button(root, text ="Set Schedule", command = SetSchedule, width = 25)
	SetPrB.place(x=50,y=530)

	# DATA download stuff
	label = tkinter.Label(root, text='Internal memory contains %d data points' % (int(OSCD[0],16)//16), font='courier')
	label.place(x=40, y=570)
	SdwlndB = tkinter.Button(root, text ="Basic Download", command = lambda: DataDownload('B'), width = 25)
	SdwlndB.place(x=50,y=600)
	AdwnldB = tkinter.Button(root, text ="Advanced Download", command = lambda: DataDownload('A'), width = 25)
	AdwnldB.place(x=300,y=600)

	label = tkinter.Label(root, text='Internal memory contains %d data points' % (int(OSCD[0],16)//16), font='courier')
	label.place(x=40, y=570)
	SdwlndB = tkinter.Button(root, text ="Basic Download", command = lambda: DataDownload('B'), width = 25)
	SdwlndB.place(x=50,y=600)
	AdwnldB = tkinter.Button(root, text ="Advanced Download", command = lambda: DataDownload('A'), width = 25)
	AdwnldB.place(x=300,y=600)

	# Close BUTTON
	quitB = tkinter.Button(root, text ="quit", command = QUIT, width = 25)
	quitB.place(x=300,y=640)

	root.mainloop()

if 'ERG-TP-' in DevInfo[0]:
	root = tkinter.Tk()
	root.title('ERG-LOGGER, v 1.0')
	root.protocol('WM_DELETE_WINDOW', QUIT)
	root.geometry('570x820')
	# device information
	label = tkinter.Label(root, text='The device '+DevInfo[0]+' (SN: '+DevInfo[1]+') is ON-LINE', font='courier')
	label.place(x=20, y=20)
	DevInfoB = tkinter.Button(root, text ="Device Information", command = lambda: ShowDevInfo(DevInfo), width = 25)
	DevInfoB.place(x=50,y=50)
	DevInfoB = tkinter.Button(root, text ="Calibration Coefficients", command = lambda: ShowCalibration(DevInfo), width = 25)
	DevInfoB.place(x=300,y=50)
	# time
	label = tkinter.Label(root, text='Date and Time settings and information:', font='courier')
	label.place(x=20, y=100)
	TimeInfoB = tkinter.Button(root, text ="Date & Time Settings", command = ShowTime, width = 25)
	TimeInfoB.place(x=50,y=130)
	# programm frequency
	label = tkinter.Label(root, text='data collection schedule:', font='courier')
	label.place(x=20, y=180)
	label = tkinter.Label(root, text='TEMPERATURE frequency (every X seconds):', font='courier')
	label.place(x=40, y=210)
	freqT = tkinter.Spinbox(root, from_ = 1, to = 86400, width = 5, justify='center', textvariable = tkinter.IntVar(value=int(OSCD[5])))
	freqT.place(x=460, y=212)
	label = tkinter.Label(root, text='   PRESSURE frequency (every X seconds):', font='courier')
	label.place(x=40, y=240)
	freqP = tkinter.Spinbox(root, from_ = 1, to = 86400, width = 5, justify='center', textvariable = tkinter.IntVar(value=int(OSCD[6])))
	freqP.place(x=460, y=242)
	# programm START
	label = tkinter.Label(root, text='data collection START date and time', font='courier')
	label.place(x=40, y=270)
	label = tkinter.Label(root, text='day:', font='courier')
	label.place(x=60, y=300)
	sday = tkinter.Spinbox(root, from_ = 1, to = 31, width = 5, justify='center', textvariable = tkinter.IntVar(value=OST.tm_mday))
	sday.place(x=100, y=302)
	label = tkinter.Label(root, text='month:', font='courier')
	label.place(x=170, y=300)
	smnth = tkinter.Spinbox(root, from_ = 1, to = 12, width = 5, justify='center', textvariable = tkinter.IntVar(value=OST.tm_mon))
	smnth.place(x=230, y=302)
	label = tkinter.Label(root, text='year:', font='courier')
	label.place(x=310, y=300)
	syear = tkinter.Spinbox(root, from_ = 1900, to = 3000, width = 5, justify='center', textvariable = tkinter.IntVar(value=OST.tm_year))
	syear.place(x=360, y=302)
	label = tkinter.Label(root, text='hour:', font='courier')
	label.place(x=50, y=330)
	shour = tkinter.Spinbox(root, from_ = 0, to = 24, width = 5, justify='center', textvariable = tkinter.IntVar(value=OST.tm_hour))
	shour.place(x=100, y=332)
	label = tkinter.Label(root, text='min:', font='courier')
	label.place(x=190, y=330)
	smin = tkinter.Spinbox(root, from_ = 0, to = 60, width = 5, justify='center', textvariable = tkinter.IntVar(value=OST.tm_min))
	smin.place(x=230, y=332)
	label = tkinter.Label(root, text='sec:', font='courier')
	label.place(x=320, y=330)
	ssec = tkinter.Spinbox(root, from_ = 0, to = 60, width = 5, justify='center', textvariable = tkinter.IntVar(value=OST.tm_sec))
	ssec.place(x=360, y=332)
	# programm STOP
	label = tkinter.Label(root, text='data collection FINISH date and time', font='courier')
	label.place(x=40, y=370)
	label = tkinter.Label(root, text='day:', font='courier')
	label.place(x=60, y=400)
	fday = tkinter.Spinbox(root, from_ = 1, to = 31, width = 5, justify='center', textvariable = tkinter.IntVar(value=OFN.tm_mday))
	fday.place(x=100, y=402)
	label = tkinter.Label(root, text='month:', font='courier')
	label.place(x=170, y=400)
	fmnth = tkinter.Spinbox(root, from_ = 1, to = 12, width = 5, justify='center', textvariable = tkinter.IntVar(value=OFN.tm_mon))
	fmnth.place(x=230, y=402)
	label = tkinter.Label(root, text='year:', font='courier')
	label.place(x=310, y=400)
	fyear = tkinter.Spinbox(root, from_ = 1900, to = 3000, width = 5, justify='center', textvariable = tkinter.IntVar(value=OFN.tm_year))
	fyear.place(x=360, y=402)
	label = tkinter.Label(root, text='hour:', font='courier')
	label.place(x=50, y=430)
	fhour = tkinter.Spinbox(root, from_ = 0, to = 24, width = 5, justify='center', textvariable = tkinter.IntVar(value=OFN.tm_hour))
	fhour.place(x=100, y=432)
	label = tkinter.Label(root, text='min:', font='courier')
	label.place(x=190, y=430)
	fmin = tkinter.Spinbox(root, from_ = 0, to = 60, width = 5, justify='center', textvariable = tkinter.IntVar(value=OFN.tm_min))
	fmin.place(x=230, y=432)
	label = tkinter.Label(root, text='sec:', font='courier')
	label.place(x=320, y=430)
	fsec = tkinter.Spinbox(root, from_ = 0, to = 60, width = 5, justify='center', textvariable = tkinter.IntVar(value=OFN.tm_sec))
	fsec.place(x=360, y=432)

	MSG =	'ACHTUNG!!!\n'+\
		'User MUST download the DATA before setting of\n'+\
		'the SCHEDULE. Otherwise the DATA can be downloaded\n'+\
		'only in the ADVANCED DOWNLOAD mode'
	label = tkinter.Label(root, text=MSG, font='courier',justify='left')
	label.place(x=40, y=470)
	SetPrB = tkinter.Button(root, text ="Set Schedule", command = SetSchedule, width = 25)
	SetPrB.place(x=50,y=560)

	# DATA download stuff
	if int(OSCD[1],16) >= InitAddrP:
		label = tkinter.Label(root, text='Internal memory contains\n%7d TEMPERATURE data points and\n%7d    PRESSURE data points' % (int(OSCD[0],16)//16, (int(OSCD[1],16) - InitAddrP)//8), font='courier',justify='left')
	else:
		label = tkinter.Label(root, text='Internal memory contains\n%7d TEMPERATURE data points and\n%7d    PRESSURE data points' % (int(OSCD[0],16)//16, 0), font='courier',justify='left')
	label.place(x=40, y=600)
	Tdwnl = IntVar()
	Pdwnl = IntVar()
	Tcheck = tkinter.Checkbutton(root, text=' - Temperature', variable=Tdwnl, onvalue=1, offvalue=0)
	Tcheck.place(x=50,y=670)
	Pcheck = tkinter.Checkbutton(root, text=' - Pressure',    variable=Pdwnl, onvalue=1, offvalue=0)
	Pcheck.place(x=50,y=690)

	SdwlndB = tkinter.Button(root, text ="Basic Download", command = lambda: DataDownload('B'), width = 25)
	SdwlndB.place(x=50,y=720)
	AdwnldB = tkinter.Button(root, text ="Advanced Download", command = lambda: DataDownload('A'), width = 25)
	AdwnldB.place(x=300,y=720)

	# Close BUTTON
	quitB = tkinter.Button(root, text ="quit", command = QUIT, width = 25)
	quitB.place(x=300,y=760)

	root.mainloop()
# fucken graph
#fig = figure(figsize=(6.2,4))
#fig.add_subplot(111).plot([1,2,3],[3,5,7])
#title('Temperature Dynamics')
#xlabel('Date')
#ylabel('Temperature, C$^\circ$')
#canvas = FigureCanvasTkAgg(fig, master=root)
#toolbar = NavigationToolbar2TkAgg(canvas, root)
#toolbar.update()
#canvas.get_tk_widget().place(x=10, y=170)
#close(fig)
