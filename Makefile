all:main.o APP_abnormal.o APP_control.o APP_handle.o APP_thread.o driver_handle.o APP_BDW_agree.o APP_ECU_agree.o APP_LMAC.o APP_UDP_agree.o driver_UDP.o APP_chassis.o APP_FOL.o APP_SXLD_agree.o driver_CAN.o
	g++ -o main main.o APP_abnormal.o APP_control.o APP_handle.o APP_thread.o driver_handle.o APP_BDW_agree.o APP_ECU_agree.o APP_LMAC.o APP_UDP_agree.o driver_UDP.o APP_chassis.o APP_FOL.o APP_SXLD_agree.o driver_CAN.o -lpthread
	
main.o: main.cpp APP_abnormal.h APP_control.h APP_handle.h APP_thread.h driver_handle.h APP_BDW_agree.h APP_ECU_agree.h APP_LMAC.h APP_UDP_agree.h driver_UDP.h APP_chassis.h APP_FOL.h APP_SXLD_agree.h driver_CAN.h
	g++ -c main.cpp

APP_abnormal.o: APP_abnormal.cpp APP_abnormal.h
	g++ -c APP_abnormal.cpp

APP_control.o: APP_control.cpp APP_control.h
	g++ -c APP_control.cpp

APP_handle.o: APP_handle.cpp APP_handle.h
	g++ -c APP_handle.cpp
	
APP_thread.o: APP_thread.cpp APP_thread.h
	g++ -c APP_thread.cpp
	
driver_handle.o: driver_handle.cpp driver_handle.h
	g++ -c driver_handle.cpp
		
APP_BDW_agree.o: APP_BDW_agree.cpp APP_BDW_agree.h
	g++ -c APP_BDW_agree.cpp
	
APP_ECU_agree.o: APP_ECU_agree.cpp APP_ECU_agree.h
	g++ -c APP_ECU_agree.cpp	
	
APP_LMAC.o: APP_LMAC.cpp APP_LMAC.h
	g++ -c APP_LMAC.cpp	

APP_UDP_agree.o: APP_UDP_agree.cpp APP_UDP_agree.h
	g++ -c APP_UDP_agree.cpp	

driver_UDP.o: driver_UDP.cpp driver_UDP.h
	g++ -c driver_UDP.cpp
	
APP_chassis.o: APP_chassis.cpp APP_chassis.h
	g++ -c APP_chassis.cpp	
	
APP_FOL.o: APP_FOL.cpp APP_FOL.h
	g++ -c APP_FOL.cpp
	
APP_SXLD_agree.o: APP_SXLD_agree.cpp APP_SXLD_agree.h
	g++ -c APP_SXLD_agree.cpp
	
driver_CAN.o: driver_CAN.cpp driver_CAN.h
	g++ -c driver_CAN.cpp
	
clean:
	-rm main main.o APP_abnormal.o APP_control.o APP_handle.o APP_thread.o driver_handle.o APP_BDW_agree.o APP_ECU_agree.o APP_LMAC.o APP_UDP_agree.o driver_UDP.o APP_chassis.o APP_FOL.o APP_SXLD_agree.o driver_CAN.o
