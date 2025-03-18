# Initialize Client and Server
all: 
	gcc Exp9_FTP_Server.c -o Exp9_FTP_Server
	gcc Exp9_FTP_Client.c -o Exp9_FTP_Client

start: 
	gnome-terminal --tab --title="FTP Server" -- bash -c "./Exp9_FTP_Server; exec bash"
	gnome-terminal --tab --title="FTP Client" -- bash -c "./Exp9_FTP_Client; exec bash"
	# konsole --new-tab -e bash -c "./Exp9_FTP_Server; exec bash" &
	# sleep 1
	# konsole --new-tab -e bash -c "./Exp9_FTP_Client; exec bash" &

clean:
	rm Exp9_FTP_Server Exp9_FTP_Client received_file.txt
