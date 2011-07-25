#include <iostream>
#include <string>
#include "MainWindow.h"

#define curSong (playlist[curTrack].substr(0,playlist[curTrack].length()-3)+"wav").c_str()


    /*
     QMessageBox msgBox;
     msgBox.setText("Leaving");
     msgBox.exec();
     */

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
	setWindowIcon(QIcon(":/Icon/GLUI.png"));
	glWindow = new GLWidget(0);
	myPlayer = new Player(0);
	myWave = NULL;
	curTrack = -1;
	numTracks = 0;
	
	setCentralWidget(glWindow);
	connect(this, SIGNAL(newSong(Wave*)), myPlayer, SLOT(playNewSong(Wave*)));
	connect(this, SIGNAL(newSong(Wave*)), glWindow, SLOT(playNewSong(Wave*)));	
	connect(glWindow, SIGNAL(songEnded()), this, SLOT(gotoNextSong()));
	
	createActions();
	createMenus();
	createToolBar();
}

MainWindow::~MainWindow()
{
	delete myWave;
	myWave = NULL;
}

void MainWindow::createActions()
{
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
    
    showPlaylistAct = new QAction(style()->standardIcon(QStyle::SP_FileIcon), "Show Play&list", this);
    showPlaylistAct->setShortcut(tr("Ctrl+L"));
    //showPlaylistAct->setStatusTip(tr("Displays the current playlist"));
    connect(showPlaylistAct, SIGNAL(triggered()), this, SLOT(showPlaylist()));
    
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
{
	fileMenu = menuBar()->addMenu("&File");
	fileMenu->addAction(openAct);
	fileMenu->addAction(enqueueAct);
	fileMenu->addSeparator();
	fileMenu->addAction(quitAct);
    
    visMenu = menuBar()->addMenu("&Visualization");
    visMenu->addAction(redAct);
    visMenu->addAction(greenAct);
    visMenu->addAction(blueAct);    
    visMenu->addSeparator();  
    visMenu->addAction(prevVisAct);   
    visMenu->addAction(nextVisAct);    
    visMenu->addSeparator();
    visMenu->addAction(fullScreenAct);
    
    playlistMenu = menuBar()->addMenu("&Playlist");
    playlistMenu->addAction(repeatOneAct);
    playlistMenu->addAction(repeatAllAct);
    playlistMenu->addSeparator();
    playlistMenu->addAction(showPlaylistAct);
    
    helpMenu = menuBar()->addMenu("&Help");
    helpMenu->addAction(aboutAct); 
}

void MainWindow::createToolBar()
{
	slider = new QSlider(Qt::Horizontal);
	slider->setRange(0,1000000);
	slider->setValue(0);
	slider->setTracking(false);
	slider->setEnabled(false);  //(Don't let this work because SDL_mixer sucks.)
	
	connect(glWindow, SIGNAL(timePassed(int)), slider, SLOT(setValue(int)));
	connect(slider, SIGNAL(valueChanged(int)), myPlayer, SLOT(scrub(int)));
	
	controls = new QToolBar();
	addToolBar(Qt::BottomToolBarArea, controls);
	controls->addAction(prevAct);
	controls->addAction(playAct);
	controls->addAction(pauseAct);
	controls->addAction(nextAct);
	controls->addAction(stopAct);
	controls->addWidget(slider);
	controls->addAction(showPlaylistAct);
	
	controls->setMinimumHeight(40);
	controls->setMovable(false);
}

void MainWindow::open()
{
	curTrack = -1;
	numTracks = 0;

	addToPlaylist();
}

void MainWindow::addToPlaylist()
{
	QString qfileName = QFileDialog::getOpenFileName(this);
	string fileName = qfileName.toStdString();
	string extension = fileName.substr(fileName.length()-3, 3);
	ifstream inFile;
	
	if (extension != "wav" && extension != "m3u" && extension != "mp3")
	{
		QMessageBox msgBox(QMessageBox::Warning, "Invalid File", "Selected file is not of type .wav, .mp3, or .m3u");
		msgBox.exec();
		return;
	}
	
	if (extension == "mp3")
		fileName = createWavOrMP3(fileName)+".mp3";
	
	if (extension == "wav" || extension == "mp3")
	{
		if(numTracks == playlist.size())
			playlist.push_back(fileName);
		else
			playlist[numTracks] = fileName;
		numTracks++;
	}
	else
	{
		inFile.open(fileName.c_str());
		while(getline(inFile, fileName))
		{
			extension = fileName.substr(fileName.length()-3, 3);
			if (extension == "mp3")
				fileName = createWavOrMP3(fileName)+".mp3";
				
			if(numTracks == playlist.size())
				playlist.push_back(fileName);
			else
				playlist[numTracks] = fileName;
			numTracks++;
		}
		inFile.close();
	}
	
	if (curTrack == -1)
		gotoNextSong();	
}

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
}

void MainWindow::gotoPrevSong()
{
	if (repeatOneAct->isChecked())
		emit newSong(myWave);
	else if (curTrack > 0 || repeatAllAct->isChecked())
	{
		curTrack--;
		
		if (curTrack < 0)
			curTrack = numTracks-1;
		
		if (myWave)
			delete myWave;
			
		myWave = new Wave(curSong);
		slider->setRange(0, myWave->GetSongLength()*1000);
		
		setWindowTitle(("GLUI - " + QFileInfo(QString(playlist[curTrack].c_str()))
						.fileName().toStdString()).c_str());
						
		emit newSong(myWave);
	}
	else
	{
		glWindow->stopSong();
		myPlayer->stop();
		curTrack = -1;
	}
}

void MainWindow::gotoNextSong()
{
	if (repeatOneAct->isChecked())
		emit newSong(myWave);
	else if (curTrack < numTracks-1 || repeatAllAct->isChecked())
	{
		curTrack++;
		curTrack %= numTracks;
		
		if (myWave)
			delete myWave;
			
		myWave = new Wave(curSong);
		slider->setRange(0, myWave->GetSongLength()*1000);
		
		setWindowTitle(("GLUI - " + QFileInfo(QString(playlist[curTrack].c_str()))
						.fileName().toStdString()).c_str());
						
		emit newSong(myWave);
	}
	else
		curTrack = -1;	
}

void MainWindow::resumeTest()
{
	if (curTrack != -1)
	{
		myPlayer->resume();
		glWindow->resumeSong();
	}
	else if (numTracks != 0)
	{
		curTrack++;
		if (myWave)
			delete myWave;
		myWave = new Wave(curSong);
		emit newSong(myWave);
	}
}

void MainWindow::quit()
{
	string cmd;
	for (int i=0;  i < numTracks; i++)
	{
		if(playlist[i].substr(playlist[i].length()-3, 3) == "mp3")
		{
			cmd = "rm -f \"" + playlist[i].substr(0,playlist[i].length()-3) + "wav\"";
			system(cmd.c_str());
		}
	}
		
	exit(0);
}

void MainWindow::fullScreen()
// POST: Player is running in full screen mode. Toolbar is hidden.
{
	if (fullScreenAct->isChecked())
	{
		controls->hide();               // hide the toolbar
		//menuBar()->hide();			//this doesn't work
		setCursor(Qt::BlankCursor);
		showFullScreen();
	}
	else
	{
		controls->show();               // show the toolbar
		//menuBar()->hide();			//this doesn't work
		setCursor(Qt::ArrowCursor);
		showNormal();
	}
}

void MainWindow::about()
{
    QMessageBox::about(this, tr("GLUI"),
             tr("GLUI.\nCreated Summer 2009 by Evan Fox and Doug Hogan."));
}

void MainWindow::contextMenuEvent(QContextMenuEvent* event)
{
	QMenu menu(this);
	menu.addAction(redAct);
	menu.addAction(greenAct);
	menu.addAction(blueAct);
	menu.addSeparator();
	menu.addAction(nextVisAct);
	menu.addAction(prevVisAct);
	menu.addSeparator();
	menu.addAction(fullScreenAct);
	
	menu.exec(event->globalPos());
}

string MainWindow::createWavOrMP3(string fileName)
{
	string fileBase = fileName.substr(0, fileName.length()-4);
	string extension = fileName.substr(fileName.length()-3, 3);
	string cmd;
	
    if(extension == "mp3")
    {
        cout << "Converting mp3 to wav temporarily..." << endl;

        cmd = "ffmpeg -y -i \"" + fileName
                  + "\" -acodec pcm_s16le \"" + fileBase + ".wav\"";
    }
    else
    {
    	cout << "Converting wav to mp3 temporarily..." << endl;

        cmd = "ffmpeg -y -i \"" + fileName
                  + "\" -acodec libmp3lame -ab 128k \"" + fileBase + ".mp3\"";
    }
    
    if(!system(cmd.c_str()))                                         //try to convert mp3 to wav
           cout << "";
   
    return fileBase;
}
