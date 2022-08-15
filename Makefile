sources=app.cpp discovery_subservice.cpp information_subservice.cpp interface_subservice.cpp machine.cpp management_subservice.cpp participant_subservice.cpp wakeup_subservice.cpp

app:	$(sources)
	g++ -o app app.cpp -pthread

clean:
	rm app
