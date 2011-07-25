// MainWindow -- main window of the user interface in Qt for visualizing sound with OpenGL and SDL.
// Summer 2009, Evan Fox and Doug Hogan

#include <iostream>
#include <string>
#include "MainWindow.h"

#define curSong (playlist[curTrack].substr(0,playlist[curTrack].length()-3)+"wav").c_str()
//Used for the filename of the current track in some methods. We always want to be playing the .wav
//  file, so this extracts the base filename (all but last three characters) with substr and replaces
//  them with "wav."


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
//PRE: parent points to an initialized QWidget
//POST: The application's GUI elements are created (menus, dock, toolbar), and we are set up to
//		start playing songs
{
	setWindowIcon(QIcon(":/Icon/GLUI.png"));		//Set Icon to image as located by .qrc file
	
	glWindow = new GLWidget(0);						//Initialize member pointer variables
	myPlayer = new Player(0);
	playlistWidget = new QListWidget();	
	myWave = NULL;	
    
    playlistWidget->clear();                        //Clear playlist initially
									
	curTrack = -1;									//-1 indicates no track playing
	numTracks = 0;									//we have no track list
	
	setCentralWidget(glWindow);						//every QMainWindow needs a central widget
													//(the one that is the primary focus)
														
	connect(this, SIGNAL(newSong(Wave*)), 			//When we start playing a new song,
			myPlayer, SLOT(playNewSong(Wave*)));	//let the SDL music player
	connect(this, SIGNAL(newSong(Wave*)),
		    glWindow, SLOT(playNewSong(Wave*)));	//and GLWidget play the new song
	
	connect(glWindow, SIGNAL(songEnded()), 			//When the current song has ended,
			this, SLOT(gotoNextSong()));			// go to the next song
	
	connect(playlistWidget, SIGNAL(currentRowChanged(int)), //Handle the playlist being clicked
		    this, SLOT(listClicked(int)));
	
	createDock();									//Create the GUI components
	createActions();
	createMenus();
	createToolBar();
	//playlistDock->toggleViewAction()->trigger(); (This doesn't work for hiding playlist upon start)
}

MainWindow::~MainWindow()
//POST: Dynamically allocated memory not handled by Qt is freed.
{
	delete myWave;
	myWave = NULL;
}

void MainWindow::createDock()
//POST: Creates a playlist widget dock that allows for our playlist to be docked on either the
//		left or right side of our window.
{
	playlistDock = new QDockWidget(tr("Playlist"), this);	//Create a new playlist dock
	
	playlistDock->setAllowedAreas(Qt::RightDockWidgetArea 	//Allow the playlist to be docked on the
								| Qt::LeftDockWidgetArea);	//left and right
								
	playlistDock->toggleViewAction()->setIcon(style()		//Set the action corresponding to whether
		        ->standardIcon(QStyle::SP_FileIcon));		//our dock is visible or not to have a
		        											//list-like icon
		        
	playlistDock->setWidget(playlistWidget);				//attach the playlist to this dock
	addDockWidget(Qt::RightDockWidgetArea, playlistDock);	//add the widget to our main window
}

void MainWindow::createActions()
//POST: Actions are initialized and connected to the appropriate slots for handling 
{
	//This is very repetitive. Format for creating an action:
	/*
		actionPointer = new QAction("n&ame", parent);			//Initiliaze action of name
																// "name" with alt-shortcut indicated
																// by character following '&'
		actionPointer->setShorcut(tr("keyboard shortcut"));		//Sets the action's shortcut
																// ex. "Ctrl+Alt+K"
		connect(actionPointer, SIGNAL(triggered),				//When the action is triggered (via menu
			    someWidgetPointer, SLOT(someSlot()));			// click et al.), handle this via a slot
	*/													        //method of someWidget
	//If the action just switches some property on or off (true or false), one can use
	//  actionPointer->setCheckable() to indicate this. To check if the property is on or off
	//  use actionPointer->isChecked()
	
    // File menu actions
	openAct = new QAction("&Open", this);
    openAct->setShortcut(tr("Ctrl+O"));
	connect(openAct, SIGNAL(triggered()), this, SLOT(open()));
	
	enqueueAct = new QAction("&Add to Playlist", this);
	enqueueAct->setShortcut(tr("Ctrl+A"));
	connect(enqueueAct, SIGNAL(triggered()), this, SLOT(addToPlaylist()));
	
	quitAct = new QAction("&Quit", this);
    quitAct->setShortcut(tr("Ctrl+Q"));
	connect(quitAct, SIGNAL(triggered()), this, SLOT(quit()));
    
    // Controls toolbar actions
	pauseAct = new QAction(style()->standardIcon(QStyle::SP_MediaPause), tr("P&ause"), this);
	connect(pauseAct, SIGNAL(triggered()), myPlayer, SLOT(pause()));
	connect(pauseAct, SIGNAL(triggered()), glWindow, SLOT(pauseSong()));
	
	playAct = new QAction(style()->standardIcon(QStyle::SP_MediaPlay), "P&lay", this);
	connect(playAct, SIGNAL(triggered()), this, SLOT(resumeTest()));
	
	prevAct = new QAction(style()->standardIcon(QStyle::SP_MediaSkipBackward), "&Prev", this);
	connect(prevAct, SIGNAL(triggered()), this, SLOT(gotoPrevSong()));
	
	nextAct = new QAction(style()->standardIcon(QStyle::SP_MediaSkipForward), "&Next", this);
	connect(nextAct, SIGNAL(triggered()), this, SLOT(gotoNextSong()));
	
	stopAct = new QAction(style()->standardIcon(QStyle::SP_MediaStop), "&Stop", this);
	connect(stopAct, SIGNAL(triggered()), myPlayer, SLOT(stop()));
	connect(stopAct, SIGNAL(triggered()), glWindow, SLOT(stopSong()));
    
    // Visualization menu actions
    redAct = new QAction("Increase &Red", this);
    redAct->setShortcut(tr("Ctrl+R"));
    connect(redAct, SIGNAL(triggered()), glWindow, SLOT(IncreaseRed())); 
    
    greenAct = new QAction("Increase &Green", this);
    greenAct->setShortcut(tr("Ctrl+G"));
    connect(greenAct, SIGNAL(triggered()), glWindow, SLOT(IncreaseGreen())); 
    
    blueAct = new QAction("Increase &Blue", this);
    blueAct->setShortcut(tr("Ctrl+B"));
    connect(blueAct, SIGNAL(triggered()), glWindow, SLOT(IncreaseBlue()));
    
    prevVisAct = new QAction("&Previous Visualization", this);
    prevVisAct->setShortcut(tr("Ctrl+Z"));
    connect(prevVisAct, SIGNAL(triggered()), glWindow, SLOT(LastVisualization())); 
           
    nextVisAct = new QAction("&Next Visualization", this);
    nextVisAct->setShortcut(tr("Ctrl+X"));
    connect(nextVisAct, SIGNAL(triggered()), glWindow, SLOT(NextVisualization()));
    
    fullScreenAct = new QAction("&Fullscreen", this);
    fullScreenAct->setShortcut(tr("Ctrl+F"));
    fullScreenAct->setCheckable(true);
    connect(fullScreenAct, SIGNAL(triggered()), this, SLOT(fullScreen()));
    
    //Playlist actions
    repeatOneAct = new QAction("Repeat &One", this);
    repeatOneAct->setShortcut(tr("Ctrl+T"));
    repeatOneAct->setCheckable(true);
    
    repeatAllAct = new QAction("Repeat &All", this);
    repeatAllAct->setShortcut(tr("Ctrl+Alt+T"));
    repeatAllAct->setCheckable(true);
    
    // About action
    aboutAct = new QAction("&About", this);
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));
}

void MainWindow::createMenus()
//POST: Menubar is created with "File," "Visualization," "Playlist", and "Help" menus with 
//      connections to appropriate actions
{
	fileMenu = menuBar()->addMenu("&File");		//Add a new menu to our QMainWindow::menuBar
												//called "File" with alt-shortcut 'F'
	fileMenu->addAction(openAct);				//Add appropriate actions as menu items (in order)
	fileMenu->addAction(enqueueAct);
	fileMenu->addSeparator();					//Add a menu separator
	fileMenu->addAction(quitAct);
    
    visMenu = menuBar()->addMenu("&Visualization");	//See above
    visMenu->addAction(redAct);
    visMenu->addAction(greenAct);
    visMenu->addAction(blueAct);    
    visMenu->addSeparator();  
    visMenu->addAction(prevVisAct);   
    visMenu->addAction(nextVisAct);    
    visMenu->addSeparator();
    visMenu->addAction(fullScreenAct);
    
    playlistMenu = menuBar()->addMenu("&Playlist");	//See above
    playlistMenu->addAction(repeatOneAct);
    playlistMenu->addAction(repeatAllAct);
    playlistMenu->addSeparator();
    playlistMenu->addAction(playlistDock->toggleViewAction());
    
    helpMenu = menuBar()->addMenu("&Help");			//See above
    helpMenu->addAction(aboutAct); 
}

void MainWindow::createToolBar()
//POST: Creates a toolbar on the bottom of our window with the following items: "previous," "play,"
//		"pause," "next," and "stop" buttons, a current song position slider (without scrubbing
//      functionality), and a "show playlist" toggle.
{
	slider = new QSlider(Qt::Horizontal);	//create a new slider
	slider->setRange(0,1000000);			//and set its range (values are arbitrary for now)
	slider->setValue(0);					//As no song is playing, start the slider at "time" 0.
	slider->setTracking(false);				//Only emit the "triggered" signal with the mouse is released
	slider->setEnabled(false);  			//Don't allow for scrubbing because SDL_mixer handles a music
											//set position for only MP3, OGG, and MOD files. No WAV. :'(.
	
	connect(glWindow, SIGNAL(timePassed(int)),	//Use the glWidget's timer to update the slider's position
		    slider, SLOT(setValue(int)));		//based on percentage of song time completed.
	//connect(slider, SIGNAL(valueChanged(int)),  //This would handle the scrubbing of our slider if the SDL
	//		myPlayer, SLOT(scrub(int)));		  //could handle setting the position of a WAV file.
	
	controls = new QToolBar();						//initialize our toolbar,
	addToolBar(Qt::BottomToolBarArea, controls);	//tell our QMainWindow to put it on the bottom,
	controls->addAction(prevAct);					//and add to it in order the appropriate actions/widgets
	controls->addAction(playAct);
	controls->addAction(pauseAct);
	controls->addAction(nextAct);
	controls->addAction(stopAct);
	controls->addWidget(slider);
	controls->addAction(playlistDock->toggleViewAction());
	
	controls->setMinimumHeight(40);					//Set the height of the bar to be 40 pixels
	controls->setMovable(false);					//Don't let the user move the bar
}

void MainWindow::open()
//POST: Clears our playlist, and starts playing the file returned by our file dialog
{
	cleanUp();                                      //delete wav files made by last playlist

    playlistWidget->clear();						//clear our playlist
    
	curTrack = -1;									//allow for our curTrack to be incremented to zero
	numTracks = 0;									//reset the number of tracks we have
	addToPlaylist();								//open a file dialog and add new songs to our now empty playlist
}

void MainWindow::addSong(string fileName)
//PRE: fileName is initialized to the path of a song of extension ".wav," ".mp3," or ".m4a"
//POST: Our playlist is populated with the song file located at fileName, numTracks is incremented by one.
//      If our song is not a .wav, it is converted to a .wav
{
	string extension = fileName.substr(fileName.length()-3, 3);					//get the file's extension
	
	if (extension == "mp3" || extension == "m4a")								//if file is a .mp3 or .m4a,
		fileName = createWavOrMP3(fileName)+"."+extension;							//convert it to a .wav
	
	playlistWidget->addItem(QFileInfo(QString(fileName.c_str())).fileName());	//add the song name to our playlist
																				//(stripped of directories)
																				
	if(numTracks == playlist.size())											//if playlist has reached full capacity,
		playlist.push_back(fileName);											//then we know the next element should be
																				//stored at the end (and we must prompt a
																				//resize)
	else
		playlist[numTracks] = fileName;											//otherwise, store the filename in the
																				//appropriate position in our vector.
	numTracks++;																//we've added a new track
}

void MainWindow::addToPlaylist()
//POST: Opens a file dialog. Adds the returned file (of type either .wav, .mp3, .m4a, or .m3u) to the playlist. numTracks is
//		updated to the final number of tracks. If no song is currently playing, we start playing the first song on our playlist.
{
    QFileDialog dialog(this);                                       //Dialog for opening a file
    dialog.setFileMode(QFileDialog::ExistingFiles);					//Handle multiple files (existing)
    QStringList qfileNames;                                         //Files being added
    QString qfileName;												//Current files being added
    string fileName;												//convert the QString to a string
    string extension;												//get the file's extension
    ifstream inFile;												//if the file is a playlist (m3u), we need to read it
    
    if(dialog.exec())
        qfileNames = dialog.selectedFiles();                      //Get first file user selected
	
    if(qfileNames.isEmpty())                                      //Don't proceed if user didn't select a filename
        return;
        
    for (int i=0; i < qfileNames.size(); i++)					  //Loop through each file to add
    {
    	qfileName = qfileNames[i];									//get the ith song of our list
    
   		fileName = qfileName.toStdString();							//convert it to a string			
		extension = fileName.substr(fileName.length()-3, 3);		//and get the extension
	
		if (extension != "wav" && extension != "m3u" && extension != "mp3" && extension != "m4a") //If invalid file extension,
		{
			QMessageBox msgBox(QMessageBox::Warning, "Invalid File", 							  //display warning message
								"Selected file is not of type .wav, .mp3, .m4a, or .m3u");
			msgBox.exec();																			 
		}
		else
		{
			if (extension != "m3u")											//if file is not a playlist,
				addSong(fileName);											//add the file to our playlist		
			else															//otherwise,
			{
				inFile.open(fileName.c_str());								//open the playlist file,
				while(getline(inFile, fileName))							//loop through every line,
					addSong(fileName);										//and add to our playlist every song in the file.
			
				inFile.close();
			}
		}
	}
	
	if (curTrack == -1)												//if we're not playing anything,
		gotoNextSong();												//start playing the first song of our playlist.
}

/* Form of showPlaylist that uses a simple QMessageBox and HTML to show the playlist
void MainWindow::showPlaylist()
{
     QMessageBox msgBox;
     string message;
     
     if(numTracks > 0)
     	message = "<h3>Playlist:</h3><ol>";
     else
     	message = "No playlist defined!";
     
     for (int i=0; i < numTracks; i++)
     {
     	if (i != curTrack)
     		message += "<li>" + playlist[i] + "</li>";
 		else
 			message += "<li><b>" + playlist[i] + "</b></li>";
 	 }
 	 
 	 if (numTracks > 0)
 	 	message += "</ol>";
 	 
     msgBox.setText(message.c_str());
     msgBox.exec();
}*/

void MainWindow::playCurTrack()
//POST: Starts playing the song as indicated by curTrack
{
	playlistWidget->setCurrentRow(curTrack);			//Update the current row of our playlist to show the current track
	
	if (myWave)											//if myWave is initialized,
		delete myWave;									//delete its contents
		        
	myWave = new Wave(curSong);							//initialized myWave to the current song as indicated by curTrack

	slider->setRange(0, myWave->GetSongLength()*1000);	//For convenience, the slider range is set from zero to the song
														//length in milliseconds.
	
	setWindowTitle(("GLUI - " + QFileInfo(QString(playlist[curTrack].c_str()))	//Set the window title to the current song
					.fileName().toStdString()).c_str());						//(stripped of directory structure)
	
	emit newSong(myWave);								//send the "new song to start playing" signal.
}

void MainWindow::gotoPrevSong()
//POST: If repeat-one is on, starts playing the current song from the beginning.
//		Otherwise, starts playing the previous song in the playlist, if there is one available.
//      If there is no previous track but repeat-all is on, we loop back to the last song.
{
	if (repeatOneAct->isChecked())						//if repeat-one is on,
		emit newSong(myWave);							//start the current song from the beginning
	else if (curTrack > 0 || repeatAllAct->isChecked()) //If there is a previous song or we're to repeat all,
	{
		curTrack--;										//set the current track to the previous song
		
		if (curTrack < 0)								//if there is no previous song,
			curTrack = numTracks-1;						//loop back to the end.
		
		playCurTrack();									//play the new current track
	}
	else												//otherwise,
	{
		glWindow->stopSong();							//stop everything.
		myPlayer->stop();
		curTrack = -1;
	}
}

void MainWindow::gotoNextSong()
//POST: If repeat-one is on, starts playing the current song from the beginning.
//		Otherwise, starts playing the next song in the playlist, if there is one available.
//      If there is no next track but repeat-all is on, we loop forward to the first song.
{
	if (repeatOneAct->isChecked())						//if repeat-one is on,		
		emit newSong(myWave);							//start the current song from the beginning
	else if (curTrack < numTracks-1 || repeatAllAct->isChecked()) //If there is a next song or we're to repeat all,
	{
		curTrack++;										//set the current track to the next song
		curTrack %= numTracks;							//if there is no next song, loop back to the first song.
		
		playCurTrack();
	}
	else												//otherwise,
	{
		glWindow->stopSong();							//stop everything.
		myPlayer->stop();
		curTrack = -1;
	}
}

void MainWindow::listClicked(int listNum)
//PRE: listNum < numTracks
//POST: If the current track is not already that designated by listNum, starts playing the song corresponding
//		to listNum 
{
    if(listNum != -1)                   //-1 indicates nothing is selected & nothing in list
    {
        if (repeatOneAct->isChecked())	//if someone clicks on an item of the list, 
            repeatOneAct->trigger();    //he/she doesn't want to repeat the current song
            
        if (curTrack != listNum)		//If the new list number isn't the track we're currently playing,
        {
            curTrack = listNum-1;		//switch the currently playing track to the one indicated by the
            gotoNextSong();				//list number. (gotoNextSong increments curTrack).
        }
    }
}

void MainWindow::resumeTest()
//POST: If we're currently playing a song, resumes the song.
//		Otherwise, we start playing the first song in the playlist.
{
	if (curTrack != -1)				//If we're currently in the middle of a song,
	{
		myPlayer->resume();			//resume the song, if need be.
		glWindow->resumeSong();
	}
	else if (numTracks != 0)		//otherwise,
	{
		curTrack = 0;				//start playing the first song in the playlist.
		playCurTrack();
	}
}

void MainWindow::quit()
//POST: Exits the program, deleting any generated .wav files along the way.
{
	cleanUp();                      //delete any .wav files we made
	exit(0);
}

void MainWindow::cleanUp()
//POST: Removes the .wav file corresponding to every .mp3 or .m4a file in the playlist
{
	string cmd;								//command string to be sent the system
	for (int i=0;  i < numTracks; i++)		//loop through every track in the playlist
	{
		if(playlist[i].substr(playlist[i].length()-3, 3) == "mp3" ||	//if the extension is .mp3 or .m4a,
		   playlist[i].substr(playlist[i].length()-3, 3) == "m4a")
		{		
			//TO BE DONE LATER: HANDLE WINDOWS DELETION
			cmd = "rm -f \"" + playlist[i].substr(0,playlist[i].length()-3) + "wav\"";	//delete the corresponding
			system(cmd.c_str());														//.wav
		}
	}
}

void MainWindow::fullScreen()
// POST: If we are to enter fullscreen (as indicated by whether or not our fullscreen action is checked), we
//       enter fullscren mode. Otherwise, we leave it.
{
	if (fullScreenAct->isChecked())							//Are we entering fullscreen or leaving it?
	{
		if (playlistDock->toggleViewAction()->isChecked())	//if the playlist is visible,
			playlistDock->toggleViewAction()->trigger();	//hide it.
		controls->hide();               					// hide the toolbar
		//menuBar()->hide();										//this doesn't work
		setCursor(Qt::BlankCursor);							//hide the cursor
		showFullScreen();									//enter full screen mode
	}
	else
	{
		controls->show();               					// show the toolbar
		//menuBar()->show();										//this doesn't work
		setCursor(Qt::ArrowCursor);							//show the cursor
		showNormal();										//leave full screen mode
	}
}

void MainWindow::about()
//POST: Displays an "about" message box, with information about this program.
{
    QMessageBox::about(this, tr("GLUI"),
             tr("GLUI.\nCreated Summer 2009 by Evan Fox and Doug Hogan."));
}

void MainWindow::contextMenuEvent(QContextMenuEvent* event)
//POST: Creates a context menu at the current mouse location with options for increasing the red,
//		green, or blue of the visualization, switching the visualization, or entering/leaving full screen.
{
	QMenu menu(this);				//create a new menu as a child of our main window.
	menu.addAction(redAct);			//Add the actions in order
	menu.addAction(greenAct);
	menu.addAction(blueAct);
	menu.addSeparator();			//add a separator
	menu.addAction(nextVisAct);
	menu.addAction(prevVisAct);
	menu.addSeparator();
	menu.addAction(fullScreenAct);
	
	menu.exec(event->globalPos());	//display the context menu
}

string MainWindow::createWavOrMP3(string fileName)
//PRE: The file at fileName exists and is a .wav, .mp3, or .m4a
//	   FFMPEG exists on the current machine and is configured to handle libfaac/libmp3lame
//POST: If the file is a .mp3 or .m4a, a corresponding .wav is created.
//		If the file is a .wav, a corresponding .m4a is created
//		FCTVAL == the base of the fileName (without extension)
{
	string fileBase = fileName.substr(0, fileName.length()-4);          //find the base of fileName and store it
	string extension = fileName.substr(fileName.length()-3, 3);         //find the extension and store it
	string cmd;                                                         //holder for system command string
	
    if(extension == "mp3" || extension == "m4a")                        //if the file is a .mp3 or .m4a
    {
        cout << "Converting mp3/m4a to wav temporarily..." << endl;     //display a message

        cmd = "ffmpeg -y -i \"" + fileName								//set the system command to a string that will
                  + "\" -acodec pcm_s16le \"" + fileBase + ".wav\"";	//convert the file to a .wav
    }
    else                                                                //othewise,
    {
    	cout << "Converting wav to mp3 temporarily..." << endl;         //display a message

        cmd = "ffmpeg -y -i \"" + fileName + "\" -acodec libmp3lame"	//set the system command to a string that will
        	  + " -ab 128k \"" + fileBase + ".mp3\"";					//convert the file to a .mp3
    }
    
    if(!system(cmd.c_str()))                                            //try to convert mp3 to wav
           cout << "";
   
    return fileBase;
}
