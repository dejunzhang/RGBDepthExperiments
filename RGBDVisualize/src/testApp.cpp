#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	
	ofSetFrameRate(60);
	ofSetVerticalSync(true);
	ofEnableAlphaBlending();
	ofBackground(0);

	cam.speed = 40;
	cam.autosavePosition = true;
	cam.usemouse = true;
	cam.useArrowKeys = false;
	cam.setFarClip(30000);
	cam.setScale(1, -1, 1);
	cam.targetNode.setScale(1,-1,1);
	cameraTrack.setCamera(cam);
	cam.loadCameraPosition();
	
	hiResPlayer = NULL;
	lowResPlayer = NULL;
	startRenderMode = false;
	currentlyRendering = false;
	allLoaded = false;
	playerElementAdded = false;
	presentMode = false;
	viewComps = false;
	
	shouldResetDuration = false;
	
	shouldSaveCameraPoint = false;
	shouldClearCameraMoves = false;
	
	temporalAlignmentMode = false;
	captureFramePair = false;

	sampleCamera = false;
	
	savingImage.setUseTexture(false);
	savingImage.allocate(1920,1080, OF_IMAGE_COLOR);
	
	fboRectangle = ofRectangle(250, 100, 1280*.75, 720*.75);
	fbo.allocate(1920, 1080, GL_RGB, 4);
	
	cameraTrack.lockCameraToTrack = false;
	
	newCompButton = new ofxMSAInteractiveObjectWithDelegate();
	newCompButton->setLabel("New Comp");
	newCompButton->setDelegate(this);
	newCompButton->setPosAndSize(fboRectangle.x+fboRectangle.width+25, 0, 100, 25);
	
	saveCompButton = new ofxMSAInteractiveObjectWithDelegate();
	saveCompButton->setLabel("Save Comp");
	saveCompButton->setDelegate(this);
	saveCompButton->setPosAndSize(fboRectangle.x+fboRectangle.width+25, 25, 100, 25);

	
	timeline.setup();
	timeline.setMovePlayheadOnDrag(true);
	timeline.getColors().loadColors("defaultColors.xml");
	timeline.setOffset(ofVec2f(0, ofGetHeight() - 200));
	timeline.setPageName("Main");
	
	loadCompositions();

	gui.addToggle("Draw Pointcloud", drawPointcloud);
	gui.addToggle("Draw Wireframe", drawWireframe);
	gui.addToggle("Draw Mesh", drawMesh);
	gui.addSlider("Point Size", pointSize, 1, 10);
	gui.addSlider("Line Thickness", lineSize, 1, 10);
	gui.addSlider("Edge Cull", currentEdgeCull, 1, 500);
	gui.addSlider("Z Far Clip", farClip, 2, 5000);
	gui.addSlider("Simplify", currentSimplify, 1, 8);
	
	gui.addPage("Sequence");
	gui.addToggle("Reset Cam Pos", shouldResetCamera);
	gui.addSlider("Camera Speed", cam.speed, .1, 40);
	gui.addToggle("Set Camera Point", shouldSaveCameraPoint);
	gui.addToggle("Delete Camera Moves", shouldClearCameraMoves);
	gui.addSlider("Duration", currentDuration, 2*30, 8*60*30);
	gui.addToggle("Set Duration", shouldResetDuration);
	gui.addToggle("Lock to Track", currentLockCamera);
	gui.addToggle("Video In/Out", enableVideoInOut);
	gui.addSlider("Video In", videoInPercent, 0, 1.0);
	gui.addSlider("Video Out", videoOutPercent, 0, 1.0);
	
	gui.addPage("Calibration Tweaks");
	gui.addToggle("Mirror", currentMirror);
	gui.addSlider("X Multiply Shift", currentXMultiplyShift, -75, 75);
	gui.addSlider("Y Multiply Shift", currentYMultiplyShift, -75, 75);
	gui.addToggle("TemporalAlignmentMode", temporalAlignmentMode);
	gui.addToggle("Capture Frame Pair", captureFramePair);
	
//	gui.addSlider("X Linear Shift", currentXAdditiveShift, -25, 25);
//	gui.addSlider("Y Linear Shift", currentYAdditiveShift, -35, 75);
//	gui.addSlider("X Scale", currentXScale, .75, 1.25);
//	gui.addSlider("Y Scale", currentYScale, .75, 1.25);
//	gui.addSlider("Rotation Comp", currentRotationCompensation, -10, 10);
	
	gui.addPage("Batching");
	gui.addToggle("View Comps", viewComps);
	gui.addToggle("Render Batch", startRenderMode);

	gui.loadFromXML();
	gui.toggleDraw();
	
	currentXScale = 1.0;
	currentYScale = 1.0;
	currentXAdditiveShift = 0;
	currentYAdditiveShift = 0;
	currentRotationCompensation = 0;	
	
	cam.maximumY =  120;
	cam.minimumY = -120;
	
}

#pragma mark customization
//--------------------------------------------------------------
void testApp::processDepthFrame(){
	
	for(int y = 0; y <	480; y++){
		for(int x = 0; x < 640; x++){
			int index = y*640+x;
			
			//***************************************************
			//CUSTOMIZATION: YOU CAN PROCESS YOU	R RAW DEPTH FRAME HERE
			//* 
			//* depthPixelDecodeBuffer contains the raw depth image
			//*
			//***************************************************			
			
		}
	}
}

void testApp::processGeometry(){
	//***************************************************
	//CUSTOMIZATION: YOU CAN MANIPULATE YOUR MESH HERE
	//*
	//* rendere.getMesh() contains mesh with indeces and texture coordinates 
	//*
	//***************************************************
}

void testApp::drawGeometry(){
	
	//***************************************************
	//CUSTOMIZATION: YOU CAN DRAW WHATEVER YOU WANT HERE TOO OR USE SHADERS
	//*
	//* draw whatever you want!
	//*
	//***************************************************
	
	if(drawPointcloud){
		glPointSize(pointSize);
		renderer.drawPointCloud();
	}
	
	if(drawWireframe){
		glLineWidth(lineSize);
		renderer.drawWireFrame();
	}
	
	if(drawMesh){
		renderer.drawMesh();
	}	
}

//************************************************************
///CUSTOMIZATION: Feel free to add things for interaction here
//************************************************************
//--------------------------------------------------------------
void testApp::keyPressed(int key){
	
	if(key == 'f'){
		ofToggleFullscreen();
	}
	
	if(!allLoaded) return;
	
	if(currentlyRendering){
		if(key == ' '){
			finishRender();
		}
		return;
	}
	
	if(key == ' '){
		if(lowResPlayer->getSpeed() != 0.0){
			lowResPlayer->setSpeed(0);
			stopCameraPlayback();
		}
		else{
			lowResPlayer->play();
			lowResPlayer->setSpeed(1.0);
		}		
	}
	
	if(key == 'i'){
		timeline.setCurrentTimeToInPoint();	
	}
	
	if(key == 'o'){
		timeline.setCurrentTimeToOutPoint();
	}
	

	//RECORD CAMERA
	if(key == 'R'){	
		toggleCameraRecord();
	}
	
	//PLAYBACK CAMERA
	if(key == 'P'){
		toggleCameraPlayback();
	}
	
	if(key == '\t'){
		videoTimelineElement.toggleThumbs();
		depthSequence.toggleThumbs();
	}
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){
	
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
	cam.usemouse = fboRectangle.inside(x, y);
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
	
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
	
}


//***************************************************
///Labbers: Everything below here is application logic
//***************************************************
#pragma mark application logic
//--------------------------------------------------------------
void testApp::update(){
	
	for(int i = 0; i < comps.size(); i++){
		comps[i]->load->enabled    = viewComps || !allLoaded;
		comps[i]->toggle->enabled  = viewComps || !allLoaded;
	}
	
	if(shouldResetDuration){
		timeline.setDurationInFrames(currentDuration);
		shouldResetDuration = false;
	}
	
	if(!allLoaded) return;
	
	if(currentLockCamera != cameraTrack.lockCameraToTrack){
		if(!currentLockCamera){
			cam.targetNode.setPosition(cam.getPosition());
			cam.targetNode.setOrientation(cam.getOrientationQuat());
			cam.rotationX = cam.targetXRot = cam.getHeading();
			cam.rotationY = cam.targetYRot = cam.getPitch();
			cam.rotationZ = cam.getRoll();
		}
		cameraTrack.lockCameraToTrack = currentLockCamera;
	}
	
	if(cameraTrack.lockCameraToTrack){
		cameraTrack.setTimelineInOutToTrack();
	}
	else{
		timeline.setInOutRange(ofRange(0,1));
	}
	
	
	cam.applyRotation = !cameraTrack.lockCameraToTrack;
	cam.applyTranslation = !cameraTrack.lockCameraToTrack;

	if(enableVideoInOut){
		videoTimelineElement.setInFrame(videoInPercent*lowResPlayer->getTotalNumFrames());
		videoTimelineElement.setOutFrame(videoOutPercent*lowResPlayer->getTotalNumFrames());
	}
	else{
		videoTimelineElement.setInFrame(0);
		videoTimelineElement.setOutFrame(lowResPlayer->getTotalNumFrames());		
	}
	
	if(startRenderMode){
		viewComps = false;
		saveComposition();
		for(int i = 0; i < comps.size(); i++){
			if(comps[i]->batchExport){
				loadCompositionAtIndex(i);
				break;
			}
		}
		
		startRenderMode = false;
		currentlyRendering = true;
		saveFolder = currentCompositionDirectory + "rendered/";
		ofDirectory outputDirectory(saveFolder);
		if(!outputDirectory.exists()) outputDirectory.create(true);
		hiResPlayer->play();
		hiResPlayer->setSpeed(0);
		hiResPlayer->setVolume(0);
		
		renderer.setRGBTexture(*hiResPlayer);
		renderer.setTextureScale(1.0, 1.0);
		//		currentSimplify = 1;
		currentRenderFrame = timeline.getInFrame();
		lastRenderFrame = currentRenderFrame-1;
		numFramesToRender = timeline.getOutFrame() - timeline.getInFrame();
		numFramesRendered = 0;
		currentLockCamera = cameraTrack.lockCameraToTrack = true;
	}
	
	if(currentlyRendering){
		timeline.setCurrentFrame(currentRenderFrame);
		hiResPlayer->setFrame(currentRenderFrame % hiResPlayer->getTotalNumFrames());
		hiResPlayer->update();
		
		////////
		//		char filename[512];
		//		sprintf(filename, "%s/TEST_FRAME_%05d_%05d_A.png", saveFolder.c_str(), currentRenderFrame, hiResPlayer->getCurrentFrame());
		//		savingImage.saveImage(filename);		
		//		savingImage.setFromPixels(hiResPlayer->getPixelsRef());
		//		savingImage.saveImage(filename);
		//		
		//		cout << "FRAME UPDATE" << endl;
		//		cout << "	setting frame to " << currentRenderFrame << " actual frame is " << hiResPlayer->getCurrentFrame() << endl;
		//		cout << "	set to percent " << 1.0*currentRenderFrame/hiResPlayer->getTotalNumFrames() << " actual percent " << hiResPlayer->getPosition() << endl;
		////////
		
		updateRenderer(*hiResPlayer);		
	}
	
	if(!currentlyRendering){
		lowResPlayer->update();	
		if(!temporalAlignmentMode && lowResPlayer->isFrameNew()){		
			updateRenderer(*lowResPlayer);
		}
		
		if(temporalAlignmentMode && currentDepthFrame != depthSequence.getSelectedFrame()){
			updateRenderer(*lowResPlayer);
		}
		
		if(sampleCamera){
			cameraTrack.sample();
		}
				
		if(captureFramePair){
			alignmentScrubber.registerCurrentAlignment();
			alignmentScrubber.save();
			captureFramePair = false;
		}
	}
	
	if(shouldClearCameraMoves){
		cameraTrack.getCameraTrack().reset();
		shouldClearCameraMoves = false;
	}
	
	if(shouldSaveCameraPoint){
		//cameraRecorder.sample(lowResPlayer->getCurrentFrame());
		cameraTrack.getCameraTrack().sample(timeline.getCurrentFrame());
		shouldSaveCameraPoint = false;	
	}
	
	if(shouldResetCamera){
		cam.targetNode.setPosition(ofVec3f());
		cam.targetNode.setOrientation(ofQuaternion());
		cam.targetXRot = -180;
		cam.targetYRot = 0;
		cam.rotationZ = 0;
		shouldResetCamera = false;
	}
	
//	newCompButton->enabled  = viewComps;
//	saveCompButton->enabled = viewComps;
	
	if(currentXAdditiveShift != renderer.yshift ||
	   currentYAdditiveShift != renderer.yshift ||
	   currentXMultiplyShift != renderer.xmult ||
	   currentYMultiplyShift != renderer.ymult ||
	   currentXScale != renderer.xscale ||
	   currentYScale != renderer.yscale ||
	   currentRotationCompensation != renderer.rotationCompensation ||
	   currentSimplify != renderer.getSimplification() ||
	   currentEdgeCull != renderer.edgeCull ||
	   farClip != renderer.farClip ||
	   currentMirror != renderer.mirror) {
		
		renderer.xshift = currentXAdditiveShift;
		renderer.yshift = currentYAdditiveShift;
		renderer.xmult = currentXMultiplyShift;
		renderer.ymult = currentYMultiplyShift;
		renderer.xscale = currentXScale;
		renderer.yscale = currentYScale;
//		renderer.rotationCompensation = currentRotationCompensation;
		renderer.edgeCull = currentEdgeCull;
		renderer.setSimplification(currentSimplify);
		renderer.farClip = farClip;
		renderer.mirror = currentMirror;
		renderer.update();
	}
	
	//update shaders
	renderer.fadeToWhite = timeline.getKeyframeValue("White");
}


//--------------------------------------------------------------
void testApp::updateRenderer(ofVideoPlayer& fromPlayer){
	
	if (!temporalAlignmentMode) {
		if(alignmentScrubber.getPairSequence().isSequenceTimebased()){
			long movieMillis = fromPlayer.getPosition() * fromPlayer.getDuration()*1000;
			currentDepthFrame = alignmentScrubber.getPairSequence().getDepthFrameForVideoFrame(movieMillis);
			depthSequence.selectTime(currentDepthFrame);
		}
		else {
			currentDepthFrame = alignmentScrubber.getPairSequence().getDepthFrameForVideoFrame(fromPlayer.getCurrentFrame());
			depthSequence.selectFrame(currentDepthFrame);
		}
	}
	
	
	processDepthFrame();
	
	renderer.update();
	
	processGeometry();
	
//	if(playbackCamera){
		//cameraRecorder.moveCameraToFrame(fromPlayer.getCurrentFrame());
		//cameraRecorder.moveCameraToFrame(timeline.getCurrentFrame());
//	}
	
	if(!drawPointcloud && !drawWireframe && !drawMesh){
		drawPointcloud = true;
	}	
	currentDepthFrame = depthSequence.getSelectedFrame();

}

//--------------------------------------------------------------
void testApp::draw(){
	
	ofBackground(255*.2);
	
	
	if(allLoaded){
		
		if(!viewComps){
			fbo.begin();
			ofClear(0, 0, 0);
			
			cam.begin(ofRectangle(0, 0, fbo.getWidth(), fbo.getHeight()));
			
			drawGeometry();
			
			cam.end();
			
			fbo.end();	
			
			//cout << timeline.getDrawRect().height << " tl height " << endl;
			
			if(!ofGetMousePressed(0)){
				timeline.setOffset(ofVec2f(0, ofGetHeight() - timeline.getDrawRect().height));
			}
			fboRectangle.height = (timeline.getDrawRect().y - fboRectangle.y - 20);
			fboRectangle.width = 16.0/9.0*fboRectangle.height;
			ofDrawBitmapString(currentCompositionDirectory, ofPoint(fboRectangle.x, fboRectangle.y-15));

			if(presentMode){
	//			ofBackground(0);
				fboRectangle.x = 0;
				fboRectangle.y = 0;
				fboRectangle.height = ofGetHeight();
				fboRectangle.width = 16.0/9.0*fboRectangle.height;
			}
			else {
				fboRectangle.x = 250;
				fboRectangle.y = 100;
				fboRectangle.height = (timeline.getDrawRect().y - fboRectangle.y - 20);
				fboRectangle.width = 16.0/9.0*fboRectangle.height;
				ofDrawBitmapString(currentCompositionDirectory, ofPoint(fboRectangle.x, fboRectangle.y-15));
			}

			fbo.getTextureReference().draw(fboRectangle);
			
			if(currentlyRendering){
				fbo.getTextureReference().readToPixels(savingImage.getPixelsRef());
				char filename[512];
				sprintf(filename, "%s/save_%05d.png", saveFolder.c_str(), currentRenderFrame);
				savingImage.saveImage(filename);
				
				///////frame debugging
				//		numFramesRendered++;
				//		cout << "	Rendered (" << numFramesRendered << "/" << numFramesToRender << ") +++ current render frame is " << currentRenderFrame << " quick time reports frame " << hiResPlayer->getCurrentFrame() << endl;
				//		sprintf(filename, "%s/TEST_FRAME_%05d_%05d_B.png", saveFolder.c_str(), currentRenderFrame, hiResPlayer->getCurrentFrame());
				//		savingImage.saveImage(filename);
				//		savingImage.setFromPixels(hiResPlayer->getPixelsRef());
				//		savingImage.saveImage(filename);
				//////
				
				//stop when finished
				currentRenderFrame++;
				if(currentRenderFrame > timeline.getOutFrame()){
					finishRender();
				}
			}
			
			if(sampleCamera){
				ofDrawBitmapString("RECORDING CAMERA", ofPoint(600, 10));
			}
//			if(playbackCamera){
//				ofDrawBitmapString("PLAYBACK CAMERA", ofPoint(600, 10));
//			}
		}
		
		gui.setDraw(!currentlyRendering && !presentMode);
		
		if(!presentMode && !viewComps){
			timeline.draw();
		}
		
		if(!presentMode){
			gui.draw();
		}
		
		ofSetColor(255);
	}
	
	if(viewComps){
		ofPushStyle();
		for(int i = 0; i < comps.size(); i++){
			if(comps[i]->wasRenderedInBatch){
				ofSetColor(50,200,100, 200);
				ofRect(*comps[i]->toggle);
			}
			else if(comps[i]->batchExport){
				ofSetColor(255,255,100, 200);
				ofRect(*comps[i]->toggle);
			}
		}
		ofPopStyle();
	}
	
}


#pragma mark compositions
//--------------------------------------------------------------
bool testApp::loadNewProject(){
	allLoaded = false;
	ofSystemAlertDialog("Select Someone's Name");
	ofFileDialogResult r;
	r = ofSystemLoadDialog("Get Directory", true);
	if(r.bSuccess){
		
		string currentMediaFolder = r.getPath();		
		ofDirectory dataDirectory(currentMediaFolder);
		dataDirectory.listDir();
		
		ofDirectory compBin(currentMediaFolder + "/compositions/");
		if(!compBin.exists()){
			compBin.create(true);
		}
		compBin.listDir();
		int compNumber = compBin.numFiles();
		currentCompositionDirectory = currentMediaFolder + "/compositions/comp" + ofToString(compNumber) + "/";
		
		string currentCompositionFile = currentCompositionDirectory+"compositionsettings.xml";
		projectsettings.loadFile(currentCompositionFile);

		int numFiles = dataDirectory.numFiles();
		string calibrationDirectory = "";
		videoPath = "";
		string depthImageDirectory = "";
		for(int i = 0; i < numFiles; i++){
			
			string testFile = dataDirectory.getName(i);
			if(testFile.find("calibration") != string::npos){
				calibrationDirectory = dataDirectory.getPath(i);
			}
			   
			if(testFile.find("depth") != string::npos){
				depthImageDirectory = dataDirectory.getPath(i);
			}
			
			if(testFile.find("mov") != string::npos){
			 	if(testFile.find("small") == string::npos){
					videoPath = dataDirectory.getPath(i);
				}
			}			
		}
		
		if(calibrationDirectory != "" && videoPath != "" && depthImageDirectory != ""){
			if(!loadAlignmentMatrices(calibrationDirectory)){
				ofSystemAlertDialog("Load Failed -- Couldn't Load Calibration Direcotry.");
				return false;
			}

			if(!loadVideoFile(videoPath)){
				ofSystemAlertDialog("Load Failed -- Couldn't load video.");
				return false;
			}
			
			if(!loadDepthSequence(depthImageDirectory)){
				ofSystemAlertDialog("Load Failed -- Couldn't load dpeth iamges.");
				return false;
			}
			
			
			cam.cameraPositionFile = currentCompositionDirectory + "camera_position.xml";
			cam.loadCameraPosition();
			
			ofDirectory compFolder(currentCompositionDirectory);
			if(!compFolder.exists()){
				compFolder.create(true);
			}
			
			saveComposition();
			refreshCompButtons();
			loadTimelineFromCurrentComp();
			
			allLoaded = true;
			return true;
		}
		else{
			ofSystemAlertDialog("Couldn't find one of the following: Calib==[" + calibrationDirectory + "] Video==[" + videoPath + "] Depth==[" + depthImageDirectory + "]");
		}
	}
	return false;	
}

void testApp::loadTimelineFromCurrentComp(){
	ofxTLKeyframer* white = (ofxTLKeyframer*)timeline.getElement("White");
	white->setXMLFileName( currentCompositionDirectory + "white.xml");
	white->load();	
	
	string cameraSaveFile = currentCompositionDirectory + "camera.xml";
	cameraTrack.setXMLFileName(cameraSaveFile);
	cameraTrack.setup();
	
}

//--------------------------------------------------------------
bool testApp::loadDepthSequence(string path){
	projectsettings.setValue("depthSequence", path);
	projectsettings.saveFile();
	
	depthSequence.setup();
	//depthSequence.disable();
	
	depthPixelDecodeBuffer = depthSequence.currentDepthRaw;
	renderer.setDepthImage(depthPixelDecodeBuffer);

	return depthSequence.loadSequence(path);
}

//--------------------------------------------------------------
bool testApp::loadVideoFile(string path){
	projectsettings.setValue("videoFile", path);
	projectsettings.saveFile();
	
	if(hiResPlayer != NULL){
		delete hiResPlayer;
	}
	hiResPlayer = new ofVideoPlayer();

	if(!hiResPlayer->loadMovie(path)){
		ofSystemAlertDialog("Load Failed -- Couldn't load hi res video file.");
		return false;
	}
	
	string lowResPath = ofFilePath::removeExt(path) + "_small.mov";
	if(lowResPlayer != NULL){
		delete lowResPlayer;
	}
	lowResPlayer = new ofVideoPlayer();
	if(!lowResPlayer->loadMovie(lowResPath)){
		ofSystemAlertDialog("Load Failed -- Couldn't load low res video file.");
		return false;		
	}
	string pairingsFile = ofFilePath::removeExt(path) + "_pairings.xml";
	if(!alignmentScrubber.getPairSequence().loadPairingFile(pairingsFile)){
		ofSystemAlertDialog("Load Failed -- Couldn't load pairings file.");
		return false;				
	}
	
	alignmentScrubber.setup();
	alignmentScrubber.setXMLFileName(pairingsFile);
	alignmentScrubber.videoSequence = &videoTimelineElement;
	alignmentScrubber.depthSequence = &depthSequence;
	
	renderer.setTextureScale(1.0*lowResPlayer->getWidth()/hiResPlayer->getWidth(), 
							 1.0*lowResPlayer->getHeight()/hiResPlayer->getHeight());
	
	videoThumbsPath = ofFilePath::removeExt(videoPath);
	if(!ofDirectory(videoThumbsPath).exists()){
		ofDirectory(videoThumbsPath).create(true);
	}
	
	videoTimelineElement.setup();
	
	if(!playerElementAdded){
		populateTimelineElements();
	}
	else{
		
	}
	renderer.setRGBTexture(*lowResPlayer);
	
//	timeline.setDurationInFrames(lowResPlayer->getTotalNumFrames());
	videoTimelineElement.setVideoPlayer(*lowResPlayer, videoThumbsPath);
	videoTimelineElement.setInFrame(200);
	videoTimelineElement.setOutFrame(300);
	lowResPlayer->play();
	lowResPlayer->setSpeed(0);
	
	return true;
}

void testApp::populateTimelineElements(){
	timeline.setPageName("Rendering");
	timeline.addElement("Camera", &cameraTrack);
	timeline.addElement("Video", &videoTimelineElement);
	timeline.addKeyframes("White", currentCompositionDirectory + "white.xml", ofRange(0,1.0) );
	
	timeline.addPage("Alignment", true);
	timeline.addElement("Video", &videoTimelineElement);
	timeline.addElement("Depth Sequence", &depthSequence);
	timeline.addElement("Alignment", &alignmentScrubber);
	
	timeline.setCurrentPage("Rendering");
	
	playerElementAdded = true;
}

//--------------------------------------------------------------
bool testApp::loadAlignmentMatrices(string path){
	projectsettings.setValue("alignmentDir", path);
	projectsettings.saveFile();
	
	return renderer.setup(path);
}

//--------------------------------------------------------------
void testApp::loadCompositions(){
	ofSystemAlertDialog("Select the MediaBin containing everyone's names");

	ofFileDialogResult r = ofSystemLoadDialog("Select Media Bin", true);
	if(r.bSuccess){
		mediaBinDirectory = r.getPath();
		refreshCompButtons();
	}
}

//--------------------------------------------------------------
void testApp::refreshCompButtons(){
	ofDirectory dir(mediaBinDirectory);
	dir.listDir();
	int mediaFolders = dir.numFiles();
	int currentCompButton = 0;
	int compx = 300;
	int compy = 150;
	for(int i = 0; i < mediaFolders; i++){
		
		string compositionsFolder = dir.getPath(i) + "/compositions/";
		ofDirectory compositionsDirectory(compositionsFolder);
		if(!compositionsDirectory.exists()){
			compositionsDirectory.create(true);
		}
		
		compositionsDirectory.listDir();
		int numComps = compositionsDirectory.numFiles();
		//int compx = newCompButton->x+newCompButton->width+25;
		for(int c = 0; c < numComps; c++){
			Comp* comp;
			if(currentCompButton >= comps.size()){
				comp = new Comp();
				comp->load  = new ofxMSAInteractiveObjectWithDelegate();
				comp->load->setup();
				comp->load->setDelegate(this);
				
				comp->toggle = new ofxMSAInteractiveObjectWithDelegate();
				comp->toggle->setup();
				comp->toggle->setDelegate(this);				
				comps.push_back(comp);
			}
			else{
				comp = comps[currentCompButton];
			}
			comp->batchExport = false;
			comp->wasRenderedInBatch = false;

			comp->toggle->setPosAndSize(compx, compy,25,25);
			comp->load->setPosAndSize(compx+25, compy, 300, 25);
			
			compy+=25;
			if(compy > ofGetHeight()-100){
				compy  = 150;
				compx += 325;
			}
			
			comp->fullCompPath = compositionsDirectory.getPath(c);
			vector<string> compSplit = ofSplitString(comp->fullCompPath, "/", true, true);
			string compLabel = compSplit[compSplit.size()-3] + ":" + compSplit[compSplit.size()-1];
			
			comp->load->setLabel(compLabel);
			if(currentCompositionDirectory == comp->fullCompPath){
				currentCompIndex = currentCompButton;
			}	
			currentCompButton++;
		}
	}
}

//--------------------------------------------------------------
void testApp::newComposition(){	
	loadNewProject();
}

//--------------------------------------------------------------
void testApp::saveComposition(){
	
	cam.saveCameraPosition();
	cout << "writing camera position to " << cam.cameraPositionFile << endl;
	//cameraRecorder.writeToFile(cameraSaveFile);
	projectsettings.setValue("cameraSpeed", cam.speed);
	projectsettings.setValue("xmult", currentXMultiplyShift);
	projectsettings.setValue("ymult", currentYMultiplyShift);
	projectsettings.setValue("xshift", currentXAdditiveShift);
	projectsettings.setValue("yshift", currentYAdditiveShift);
	projectsettings.setValue("xscale", currentXScale);
	projectsettings.setValue("yscale", currentYScale);
	
	projectsettings.setValue("pointSize", pointSize);
	projectsettings.setValue("lineSize", lineSize);
//	projectsettings.setValue("cameraFile", cameraSaveFile);
	projectsettings.setValue("pointcloud", drawPointcloud);
	projectsettings.setValue("wireframe", drawWireframe);
	projectsettings.setValue("mesh", drawMesh);
	projectsettings.setValue("edgeCull", currentEdgeCull);
	projectsettings.setValue("farClip",farClip);
	projectsettings.setValue("simplify",currentSimplify);
	projectsettings.setValue("mirror", currentMirror);
	projectsettings.setValue("duration", currentDuration);
	
	projectsettings.setValue("videoinout", enableVideoInOut);
	projectsettings.setValue("videoin", videoInPercent);
	projectsettings.setValue("videoout", videoOutPercent);
	
	projectsettings.saveFile();
}

void testApp::objectDidRollOver(ofxMSAInteractiveObject* object, int x, int y){
}

void testApp::objectDidRollOut(ofxMSAInteractiveObject* object, int x, int y){
}

void testApp::objectDidPress(ofxMSAInteractiveObject* object, int x, int y, int button){
}

//--------------------------------------------------------------
void testApp::objectDidRelease(ofxMSAInteractiveObject* object, int x, int y, int button){
	if(object == newCompButton){
		newComposition();
	}
	else if(object == saveCompButton){
		saveComposition();		
	}
	else {
		for(int i = 0; i < comps.size(); i++){
			
			if(comps[i]->toggle == object){
				comps[i]->wasRenderedInBatch = false;
				comps[i]->batchExport = !comps[i]->batchExport;
				
				break;
			}

			if(comps[i]->load == object){
				loadCompositionAtIndex(i);
				break;
			}		
		}
	}
}


//--------------------------------------------------------------
bool testApp::loadCompositionAtIndex(int i){
	stopCameraPlayback();
	stopCameraRecord();
	
	currentCompositionDirectory = comps[i]->fullCompPath + "/";
	currentCompIndex = i;
		
	
	cout << "loading comp " << currentCompositionDirectory << " clicked comp button is " << comps[i]->load->getLabel() << endl;
	if(projectsettings.loadFile(currentCompositionDirectory+"compositionsettings.xml")){
		//string loadedAlignmentDir = projectsettings.getValue("alignmentDir", "");
		string loadedAlignmentDir = currentCompositionDirectory + "../../calibration/";
		cout << "loading alignment " << loadedAlignmentDir << endl;
		if(!loadAlignmentMatrices(loadedAlignmentDir)){
			return false;
		}
		
		string loadedVideoFile = projectsettings.getValue("videoFile", "");
		loadedVideoFile = currentCompositionDirectory + "../../" + ofFilePath::getFileName(loadedVideoFile);
		cout << "loading video " << loadedVideoFile << endl;
		if(!loadVideoFile(loadedVideoFile)){
			return false;
		}
		
		string loadedDepthSequence = projectsettings.getValue("depthSequence", "");
		loadedDepthSequence = currentCompositionDirectory+ "../../" + ofFilePath::getFileName(loadedDepthSequence);
		cout << "loading depth " << loadedDepthSequence << endl;
		if(!loadDepthSequence(loadedDepthSequence)){
			return false;
		}
		
		cam.speed = projectsettings.getValue("cameraSpeed", 20.);
//		currentXShift = projectsettings.getValue("shiftx", 0.);
//		currentYShift = projectsettings.getValue("shifty", 0.);
		
		currentXMultiplyShift = projectsettings.getValue("xmult", 0.);
		currentYMultiplyShift = projectsettings.getValue("ymult", 0.);
		currentXAdditiveShift = projectsettings.getValue("xshift", 0.);
		currentYAdditiveShift = projectsettings.getValue("yshift", 0.);
		currentXScale = projectsettings.getValue("xscale", 1.0);
		currentYScale = projectsettings.getValue("yscale", 1.0);
		
		pointSize = projectsettings.getValue("pointSize", 1);
		lineSize = projectsettings.getValue("lineSize", 1);
		currentEdgeCull = projectsettings.getValue("edgeCull", 50);
		farClip = projectsettings.getValue("farClip", 5000);
		drawPointcloud = projectsettings.getValue("pointcloud", false);
		drawWireframe = projectsettings.getValue("wireframe", false);
		drawMesh = projectsettings.getValue("mesh", false);
		currentSimplify = projectsettings.getValue("simplify", 1);
		currentMirror = projectsettings.getValue("mirror", false);
		currentDuration = projectsettings.getValue("duration", int(videoTimelineElement.videoThumbs.size()) );
		enableVideoInOut = projectsettings.getValue("videoinout", false);
		videoInPercent = projectsettings.getValue("videoin", 0.);
		videoOutPercent = projectsettings.getValue("videoout", 1.);
		
		shouldResetDuration = true;
		
		//set keyframer files based on comp
		loadTimelineFromCurrentComp();
		
		allLoaded = true; 
	}
	
	//LOAD CAMERA SAVE AND POS
	cam.cameraPositionFile = currentCompositionDirectory + "camera_position.xml";
	cam.loadCameraPosition();
	timeline.setCurrentPage("Main");
	
	/*
	string cameraFilePath = currentCompositionDirectory + "camera.xml";
	ofFile cameraFile(cameraFilePath);
	if(cameraFile.exists()){
		cameraRecorder.loadFromFile(cameraFilePath);
		if(cameraRecorder.getSamples().size() > 0){
			timeline.setInPointAtFrame(cameraRecorder.getFirstFrame());
			timeline.setOutPointAtFrame(cameraRecorder.getLastFrame());
		}
		else{
			timeline.setInOutRange(ofRange(0,1.0));
		}
	}
	else{
		ofSystemAlertDialog("Composition " + currentCompositionDirectory + " has no camera save");
		cameraRecorder.reset();
	}
	 */
}	

//--------------------------------------------------------------
void testApp::objectDidMouseMove(ofxMSAInteractiveObject* object, int x, int y){

}

//--------------------------------------------------------------
void testApp::finishRender(){
	currentlyRendering = false;
	comps[currentCompIndex]->batchExport = false;
	comps[currentCompIndex]->wasRenderedInBatch = true;
	for(int i = currentCompIndex+1; i < comps.size(); i++){
		if(comps[i]->batchExport){
			loadCompositionAtIndex(i);
			startRenderMode = true;
			return;
		}
	}
	
	//no more left, we are done
	//stopCameraPlayback();
	renderer.setRGBTexture(*lowResPlayer);
	renderer.setTextureScale(1.0*lowResPlayer->getWidth()/hiResPlayer->getWidth(), 
							 1.0*lowResPlayer->getHeight()/hiResPlayer->getHeight());	
	
}


void testApp::startCameraRecord(){
	if(!sampleCamera){
		cameraTrack.lockCameraToTrack = false;
		//cameraRecorder.reset();
		cameraTrack.getCameraTrack().reset();
		sampleCamera = true;
		lowResPlayer->setSpeed(1.0);
	}	
}

//--------------------------------------------------------------
void testApp::stopCameraRecord(){
	sampleCamera = false;
}

//--------------------------------------------------------------
void testApp::toggleCameraRecord(){
	if(sampleCamera){
		stopCameraRecord();
	}
	else{
		startCameraRecord();
	}
}

//--------------------------------------------------------------
void testApp::stopCameraPlayback(){
//	if(playbackCamera){
//		playbackCamera = false;
//		lowResPlayer->setSpeed(0.);
//		hiResPlayer->setSpeed(0.);
//		cam.applyRotation = true;
//		cam.applyTranslation = true;
//	}
}

//--------------------------------------------------------------
void testApp::startCameraPlayback(){
//	if(cameraRecorder.getSamples().size() > 0){
//		playbackCamera = true;
//		cam.applyRotation = false;
//		cam.applyTranslation = false;
//		timeline.setInPointAtFrame(cameraRecorder.getFirstFrame());
//		timeline.setOutPointAtFrame(cameraRecorder.getLastFrame());
//		
//		if(currentlyRendering){
//			hiResPlayer->setSpeed(1.0);
//		}
//		else{
//			lowResPlayer->setSpeed(1.0);
//		}
//	}
//	else{
//		ofSystemAlertDialog("Save a camera move in order to play back");
//	}
}

//--------------------------------------------------------------
void testApp::toggleCameraPlayback(){
	cameraTrack.lockCameraToTrack = !cameraTrack.lockCameraToTrack;
//	if(playbackCamera){
//		stopCameraPlayback();
//	}
//	else {
//		startCameraPlayback();
//	}
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){
	timeline.setWidth(w);
	timeline.setOffset(ofVec2f(0, ofGetHeight() - timeline.getDrawRect().height));
}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}