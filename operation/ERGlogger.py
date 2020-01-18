#!/usr/bin/python3

import tkinter
from tkinter import messagebox,filedialog
from serial import Serial
from serial.tools import list_ports
from numpy import *
from matplotlib.pyplot import *
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg, NavigationToolbar2TkAgg
from time import *

#+++++++++++++++++++++++++++++++
# 
# DEVICE VCP COMMUNICATION CLASS
#
#+++++++++++++++++++++++++++++++
class STM:
	s = None
	def __init__(self, tty):
		self.tty = tty
		self.init()

	def init(self):
		if self.s:
			self.s.close()
		self.s = Serial(port=self.tty, timeout=120)

	def HandShake(self):
		self.s.write('hello\r'.encode())
		rpl = self.s.readline()
		rpl = int(self.s.readline().strip().decode())
		if rpl == 167321907:
			info = []
			info.append(self.s.readline().strip().decode())
			print(info[-1])
			if (info[0] == 'ERG-T-01') or (info[0] == 'ERG-T-02'):
				for p in range(11):
					info.append(self.s.readline().strip().decode())
				print(' - initial hand-shake is '+self.s.readline().strip().decode())
				return info
			elif info[0] == 'ERG-TP-01':
				for p in range(11):
					info.append(self.s.readline().strip().decode())
				print(' - initial hand-shake is '+self.s.readline().strip().decode())
				return info
		else:
			print(' - initial hand-shake failed: '+self.s.readline().strip().decode())
			return [0]

	def GetTime(self):
		Tx86 = time()
		while Tx86%int(Tx86) > 0.1:
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
		SCDL = []
		self.s.write(('GetProgramm\r').encode())
		rpl = self.s.readline()
		for p in range(4):
			SCDL.append(self.s.readline().strip().decode())
		rpl = self.s.readline().strip().decode()
		if rpl == 'OK':
			PRINT(' - current data acquisition schedule was read successfully')
			return SCDL
		else:
			PRINT(' - error in read of the current acquisition schedule')
			return [0,0,0,0]

	def SetProgramm(self, freq, start, stop):
		self.s.write(('SetProgramm %d %d %d\r' % (freq, start, stop)).encode())
		rpl = self.s.readline()
		rpl = self.s.readline().strip().decode()
		if rpl == 'OK':
			PRINT(' - the schedule was set successfully')
		else:
			PRINT(' - ERROR. Programm was NOT SET: '+rpl)
	def PowerOff(self):
		self.s.write('sleep\r'.encode())
		rpl = self.s.readline()
		rpl = self.s.readline().strip().decode()
		if rpl == 'OK':
			PRINT(' - the device was disconnected properly')
		else:
			PRINT(' - ERROR. The device was not disconected properly: '+rpl)

	# address is a three of four byte address in hexadecimal notation
	# vol - is an ammount of data to read in bytes
	# numbytes - is a number of bytes per value
	def GetData(self, fName, vol, numbytes, address = '00000000'):
		# windows stuff
		MSG1 = tkinter.Toplevel(width = 270, height = 90)
		MSG1.title('Download')
		label = tkinter.Label(MSG1, text='Downloading... Please wait!', font= ('helvetica',12,'bold'))
		label.place(x=30, y = 20)
		MSG1.update_idletasks()
		tm = time()
		# stm32 communication
		self.s.write(('SendDataToX86 %d ' % vol + address+'\r' ).encode())
		rpl = self.s.readline()
		rpl = self.s.read(vol)
		# ACHTUNG!!! Here is an extremely important check!!!
		# if some data was lost due to the connection or something else,
		# the "take a range": rpl[numbytes*p:numbytes*(p+1)] returns empty bytes sequence: b''
		# the latter will be treated as int(0)!!!
		if vol != len(rpl):
			ERR = tkinter.messagebox.showerror('Download ERROR','Sometheng was wrong. Try again, or try ADVANCED DOWNLOAD')
			MSG1.destroy()
			return
		# conversion to the integers
		D = []
		for p in range(vol//numbytes):
			tmp = int.from_bytes(rpl[numbytes*p:numbytes*(p+1)], byteorder='big')
			D.append(tmp - 4294967295*(tmp > 2147483648))
		# some logical operations and a windows stuff again! 
		SaveDataFile(D,fName)
		MSG1.destroy()
		MSG2 = tkinter.messagebox.showinfo('Download','Download complete in %.1e sec!' % (time()-tm))
		if AdwndlBox != 0:
			AdwndlBox.destroy()
		PRINT('   download completed succesfully')


#+++++++++++++++++++++++++++++++
# 
# WINDOW-SPECIFIC functions 
#
#+++++++++++++++++++++++++++++++

# This one creates a list of the COM ports and ties to communicate with them. 
# In case if the device responds correctly, it creates a serial STM class device
# Be careful, as the function will recognize and return the first appropriate device, which is an occasional choise!
def Device():
	CPlist = list_ports.comports()
	for p in range(len(CPlist)):
		dev = CPlist[p].device
		try:
			tmp = STM(tty=dev)
			info = tmp.HandShake()
			print(info)
			if info[0] != 0:
				row = (' - '+info[0]+' device defined as '+dev+' is a proper ERG-logger device and is now connected')
				return tmp, info, row
		except:
			return 0, 0, (' - device "'+dev+'" is not a proper ERG-logger device')
	ERR = tkinter.messagebox.showerror('No Device','The appropriate ERG Logger Devices were not found!\nTry to replace the USB cable :(')
	exit()

def ShowDevInfo(INFO):
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

def ShowCalibration(INFO):
	CalibrBox = tkinter.Toplevel(width = 400, height = 150)
	CalibrBox.title(INFO[0])
	label = tkinter.Label(CalibrBox, text='alpha1 = %10.4e;' % (float(INFO[6])/100000000), font='courier')
	label.place(x=20, y = 20)                                                         
	label = tkinter.Label(CalibrBox, text='alpha2 = %10.4e;' % (float(INFO[8])/100000000), font='courier')
	label.place(x=20, y = 40)                                                         
	label = tkinter.Label(CalibrBox, text='alpha3 = %10.4e;' % (float(INFO[10])/100000000), font='courier')
	label.place(x=20, y = 60)
	label = tkinter.Label(CalibrBox, text='R1 = %9.4f' % (float(INFO[7])/100000), font='courier')
	label.place(x=230, y = 20)
	label = tkinter.Label(CalibrBox, text='R2 = %9.4f' % (float(INFO[9])/100000), font='courier')
	label.place(x=230, y = 40)
	label = tkinter.Label(CalibrBox, text='R3 = %9.4f' % (float(INFO[11])/100000), font='courier')
	label.place(x=230, y = 60)
	CANCEL = tkinter.Button(CalibrBox, text ="Close",  command = CalibrBox.destroy, width = 10)
	CANCEL.place(x=140,y=110)

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
	#FRQ = 60*freq.get()
	FRQ = freq.get()
	try:
		q = int(FRQ)
	except:
		RJ = tkinter.messagebox.showerror('Frequency ERROR','Please, provide an integer value for the DATA ACQUISITION frequency!')
		return
	num = (NFN - NST)//(int(FRQ)*60)
	# check if the number of DATA point could be writen in the FLASH
	if 'MX25L64' in DevInfo[4]:
		NUM = 64*1024*1024/8/16
	elif 'MX25L128' in DevInfo[4]:
		NUM = 128*1024*1024/8/16
	elif 'MX25L256' in DevInfo[4]:
		NUM = 256*1024*1024/8/16
	else:
		RJ = tkinter.messagebox.showerror('MEMORY ERROR','The type of FLASH was not recognized. Sorry!!!')
		PRINT(' - the acquisition schedule WAS NOT set due to the FLASH recognition problem')
		return
	if num > NUM:
		AUS = tkinter.messagebox.askyesno('ACHTUNG','The number of the requested DATA points (%d) is greater than could be written in the internal memory (%d). The schedule will be truncated.\n\nWhould you like to proceed?' % (num,NUM))
		if AUS:
			num = NUM
			NFN = NST + num*int(FRQ)*60
		else:
			PRINT(' - the acquisition schedule WAS NOT set due to the too long schedule problem')
			return
	# check if the start date is placed before the finish date
	if NST > NFN:
		RJ = tkinter.messagebox.showerror('Date ERROR','The date acquisition START date must be before FINISH date!!!')
	#elif float(FRQ) != int(float(FRQ)):
	#	RJ = tkinter.messagebox.showerror('Frequency ERROR','The data acquisition FREQUENCY MUST be a multiple of a minute!!!')
	else:
		# chechs the dates if they are in the past and decides what to say to the user
		tm = time()
		if (NST < tm) and (NFN > tm):
			AUS = tkinter.messagebox.askyesno('ACHTUNG','The date of the data acquisition START is in the PAST.\nThe schedule will be shifted forward.\n\nProceed?')
			if AUS:
				# check the length of the schedule
				if (NFN - tm - 30) > (2*365*24*3600):
					RJ = tkinter.messagebox.showwarning('Schedule WARNING','You are requesting a schedule longer than two years. It could be too long for the battery cells of the device!')

				MSG = 'The DATA acquisition will start 30 sec after the USB cable disconnection\n'+\
				      'The estimated start date is\n'+ctime(tm+30)+'\n'+\
				      'The estimated finish date is\n'+ctime(tm+30 + int(FRQ)*num*60)+'\n'+\
				      'The total programm length is %.f days' % ((NFN - NST - 30)/86400)+'\n'+\
				      'There will be collected %d DATA points\n' % num + '\n\n'+\
				      'Set the schedule?'
				print(ctime(NST),ctime(NFN))
				NFN = NFN + (tm - NST)
				NST = tm
				print(ctime(NST),ctime(NFN))
			else:
				PRINT(' - the acquisition schedule WAS NOT set due to the wrong START date')
				return	
		elif NFN < tm:
			AUS = tkinter.messagebox.askyesno('ACHTUNG','The date of data acquisition FINISH is in the past.\nNo DATA will be collected!!!\n\nAre you sure?')
			if AUS:
				MSG = 'No data wil be collected!!!\n\nSet the schedule?'
			else:
				PRINT(' - the acquisition schedule WAS NOT set due to the zero-output problem')
				return
		else:
			# check the length of the schedule
			if (NFN - NST) > (2*365*24*3600):
				RJ = tkinter.messagebox.showwarning('Schedule WARNING','You are requesting a schedule longer than two years. It could be too long for the battery cells of the device!')
	
			MSG = 'The DATA acquisition will start at\n'+ctime(NST)+'\n'+\
			      'The DATA acquisition will finish at\n'+ctime(NFN)+'\n'+\
			      'The total programm length is %.f days' % ((NFN - NST)/86400)+'\n'+\
			      'There will be collected %d DATA points\n' % num + '\n\n'+\
			      'Set the schedule?'
	
		# schedule setting and log messages production
		AUS = tkinter.messagebox.askyesno('Schedule Settings', MSG)
		if AUS:
			PRINT(' - the DATA acquisition schedule settings:')
			MSG = MSG.split('\n')
			for p in range(len(MSG)-1):
				if MSG[p] != '':
					PRINT('   '+MSG[p].lower())
			
			stm32.SetProgramm(int(FRQ),NST,NFN)
			# TODO !!! It must reread the schedule and show it to the user!!!	
		else:
			PRINT(' - the acquisition schedule WAS NOT set due to the USER abort')

def DataDownload(mode):
	fname = (DevInfo[0]+'-'+DevInfo[1]+'-'+str(TM.tm_year)+'.%2d.%2d-%2d.%2d.log' % (TM.tm_mon, TM.tm_mday, TM.tm_hour, TM.tm_min)).replace(' ','0')
	fName = filedialog.asksaveasfilename(title='Choose a file', initialfile = fname)
	DATA = [0]
	if (fName != ()) and (fName != ''):
		PRINT(' - filename choosen: '+fName)
	else:
		return
	if mode == 'A':
		global AdwndlBox
		MSG =  'The ADVANCED DOWNLOAD mode allows to download\n'
		MSG += 'all the DATA from the internal memory.\n\n'
		MSG += 'User shoud provide the beginning adress,\n'
		MSG += 'and the number of the data samples do download.\n'
		MSG += 'The address is a HEXADECIMAL 24 bit value:\n'
		MSG += 'Defauls is zero, and final is FFFFFF.\n'
		MSG += 'The starting address MUST be a multiple of 16!!!\n'
		MSG += 'The amount of the data samples is limited to 524288.\n'
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
		DwnldB = tkinter.Button(AdwndlBox, text ="Download", command = lambda: stm32.GetData(fName, int(amnt.get())*16, 4, address = addr.get().replace(' ','0')), width = 25)
		DwnldB.place(x=40,y=250)
		DwnldB = tkinter.Button(AdwndlBox, text ="Close", command = AdwndlBox.destroy, width = 25)
		DwnldB.place(x=300,y=250)
	elif mode == 'B':
		#MSG = tkinter.messagebox.showerror('Download ERROR','There is no new data in the memory.\nTry the ADVANCED DOWNLOAD')
		vol = int(OSCD[0], 16)
		if vol == 0:
			RJ = tkinter.messagebox.showerror('Download ERROR','There is no new data in the memory.\nTry the ADVANCED DOWNLOAD')
		else:
			stm32.GetData(fName, vol, numbytes = 4)
	
def SaveDataFile(DATA,fName):
	tm = array(DATA[0::4])
	V1 = array(DATA[1::4])
	V2 = array(DATA[2::4])
	V3 = array(DATA[3::4])
	T1 = Tenz2Temp(V1,  float(DevInfo[6])/100000000,  float(DevInfo[7])/100000)
	T2 = Tenz2Temp(V2,  float(DevInfo[8])/100000000,  float(DevInfo[9])/100000)
	T3 = Tenz2Temp(V3, float(DevInfo[10])/100000000, float(DevInfo[11])/100000)		
	W = open(fName, 'w')
	W.write('# Temperature log-file created '+ctime()+' by ERGlogger.py\n# Device model: '+DevInfo[0]+', Device Serial Number: '+DevInfo[1]+'\n#\t\t\t\t\t\tDr. Zoidberg\n#\n')
	for p in range(len(tm)):
		W.write(ctime(tm[p])+'\t%8.5f\t%8.5f\t%8.5f\n' % (T1[p],T2[p],T3[p]))
	W.close()
	W = open(fName.replace(fName.split('/')[-1],'VOLTAGE-'+fName.split('/')[-1]), 'w')
	W.write('# Voltage (in nV) log-file created '+ctime()+' by ERGlogger.py\n# Device model: '+DevInfo[0]+', Device Serial Number: '+DevInfo[1]+'\n#\t\t\t\t\t\tDr. Zoidberg\n#\n')
	for p in range(len(tm)):
		W.write(ctime(tm[p])+'\t%9d\t%9d\t%9d\n' % (V1[p],V2[p],V3[p]))
	W.close()

def Tenz2Temp(U,A,R0):
	V  =  1250.0   # mV
	R1 =  1050.0   # Ohm
	R2 =  5000.0   # Ohm
	U = U/1000000  # to convert it from nV to mV
	#return (1/A)*( U*R1*(R1+R2)*(1/R0+1/R1) - V*(R1/R0)*(R0-R2))/(V*R1 - U*(R1 + R2))
	return ((R2/R0)*(V*R1 + U*(R1+R2))/(V*R2 - U*(R1+R2)) - 1)/A

def PRINT(row):
	print(row)
	oFile.write(row+'\n')
	oFile.flush()

def QUIT():
	stm32.PowerOff()
	exit()

#+++++++++++++++++++++++++++++++
# 
# THE MAIN BODY OF THE :) 
#
#+++++++++++++++++++++++++++++++


# VC and STM class creation
stm32, DevInfo, row = Device()
# the OPERATION log-file!
TM = strptime(ctime())
#oFile = open(('OPERATION-'+DevInfo[0]+'-'+DevInfo[1]+'-'+str(TM.tm_year)+'.%2d.%2d-%2d.%2d.log' % (TM.tm_mon, TM.tm_mday, TM.tm_hour, TM.tm_min)).replace(' ','0'),'w')
oFile = open(('OPERATION-TEST.log'),'w')
PRINT(row)
if stm32 == 0:
	exit()
# I need it to be global, as I want to destroy it after data download
AdwndlBox = 0
# old schedule acquisition
OSCD = stm32.GetProgramm()           # Old Schedule: CurrentLastAddress, Start, Stop, Frequency
OST  = strptime(ctime(int(OSCD[1]))) # Old Shedule Start Point
OFN  = strptime(ctime(int(OSCD[2]))) # Old Shedule Start Point
PRINT('   final address is: '+OSCD[0])
PRINT('   there should be %d points in the log-file' % (int(OSCD[0],16)//16))
PRINT('   old  START date:  '+ctime(int(OSCD[1])))
PRINT('   old FINISH date:  '+ctime(int(OSCD[2])))

# ROOT program window definitions
root = tkinter.Tk()
root.title('ERG-LOGGER, v 1.0')
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
label = tkinter.Label(root, text='Data acquisition schedule:', font='courier')
label.place(x=20, y=180)
label = tkinter.Label(root, text='acquisition frequency (every X minutes):', font='courier')
label.place(x=40, y=210)
freq = tkinter.Spinbox(root, from_ = 1, to = 86400, width = 5, justify='center', textvariable = tkinter.IntVar(value=int(OSCD[3])))
freq.place(x=460, y=212)
label = tkinter.Label(root, text='acquisition START date and time', font='courier')
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

label = tkinter.Label(root, text='acquisition FINISH date and time', font='courier')
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

MSG = 'ACHTUNG!!!\n'+\
      'User MUST download the DATA before setting of\n'+\
      'a SCHEDULE. Otherwise DO NOT close the program\n'+\
      'window before the DATA has been downloaded'
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

# Close BUTTON
quitB = tkinter.Button(root, text ="quit", command = QUIT, width = 25)
quitB.place(x=300,y=640)

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

root.mainloop()

