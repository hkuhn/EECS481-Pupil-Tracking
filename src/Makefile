all: pupil_tracking_main.o recording_tracking_main.o camera_tracking_main.o eigenfaces.o helper.o
	g++ pupil_tracking_main.o recording_tracking_main.o camera_tracking_main.o eigenfaces.o helper.o `pkg-config --cflags --libs opencv` -o pupil_tracker 

recording_tracking_main.o: tracking/recording_tracking_main.cpp tracking/recording_tracking_main.h
	g++ tracking/recording_tracking_main.cpp -c

camera_tracking_main.o: tracking/camera_tracking_main.cpp tracking/camera_tracking_main.h
	g++ tracking/camera_tracking_main.cpp -c

pupil_tracking_main.o: pupil_tracking_main.cpp pupil_tracking_main.h
	g++ pupil_tracking_main.cpp -c

eigenfaces.o: tracking/eigenfaces.cpp tracking/eigenfaces.hpp
	g++ tracking/eigenfaces.cpp -c

helper.o: tracking/helper.cpp tracking/helper.hpp
	g++ tracking/helper.cpp -c

clean:
	rm *.o pupil_tracker

