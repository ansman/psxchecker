/**
 * A program which is used for test the screen, sound and buttons on a psp.
 * Made for and used by PSXCare (http://www.psxcare.com/)
 *
 * I know the code is ugly as hell and very messy right now.
 * But if you want to develop this addon just mail and I'll explain as best i can.
 *
 * To compile this program OSLib and the psp sdk needs to be in your PATH
 *
 * This program is protected under GNU Free Public License.
 *
 * You may freely use the code in your own program as long as it's open source and free :)
 *
 * USE THIS PROGRAM AT YOUR OWN RISK!
 *
 * @author Xager
 * @email nicklas.ansman@gmail.com
 * @version 1.0
 * @date 17-01-2009
 */
#include <oslib/oslib.h>

static void testKeys(), initialize(), testSound(), testScreen(), displayAbout(), quit();

//1000000 must be devisable with the FPS
unsigned char __FPS__ = 50;

unsigned char __JOYSTICK_MINIMUM__ = 20;

char version[] = "1.0";

//callbacks
PSP_MODULE_INFO("PSXChecker", 0, 1, 1);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);

int main()
{
    oslInit(0);

    //Initializes the graphics with 8 bits per channel and 8 bit alpha.
    oslInitGfx(OSL_PF_8888, 1);

    //Initialization of the text console
    oslInitConsole();

	initialize();

	short selected = 0;
	bool running = 1;
	unsigned char i;

	char menuItemsText[][20] = {" Key test ", " Pixel test ", " Sound test ", " About ", " Quit "};
	unsigned short menuItemsXOffset[] = {196, 190, 190, 205, 209};

	void (*menuItemsFunctions[])() = {&testKeys, &testScreen, &testSound, &displayAbout, &quit};

	short numberOfItems = sizeof(menuItemsXOffset) / sizeof(unsigned short);


	oslReadKeys();

	//The main menu
	while(!osl_quit && running)
	{
		oslStartDrawing();

		oslCls();
		oslSetTextColor(RGB(252,154,1));
		oslSetBkColor(RGBA(0,0,0,0));
		oslPrintf_xy(132, 80, "PSXChecker version %s by Xager", version);
		oslSetTextColor(RGB(255,255,255));
		oslPrintf_xy(198, 100, "MAIN MENU");
		oslPrintf_xy(198, 110, "---------");

		for(i = 0; i < numberOfItems; i++)
		{
			if(selected == i)
			{
				oslSetTextColor(RGB(0,0,0));
				oslSetBkColor(RGBA(252,154,1,255));
			}
			else
			{
				oslSetTextColor(RGB(255,255,255));
				oslSetBkColor(RGBA(0,0,0,0));
			}

			oslPrintf_xy(menuItemsXOffset[i], (unsigned short)(120+i*10), menuItemsText[i]);
		}

		oslReadKeys();

		if (osl_keys -> pressed.up || osl_keys -> pressed.left) selected--;
		if (osl_keys -> pressed.down || osl_keys -> pressed.right) selected++;

		if (osl_keys -> pressed.cross || osl_keys -> pressed.start)
		{
			(*menuItemsFunctions[selected])();
		}

		//Makes the selection wrap around.
		if (selected >= numberOfItems) selected = numberOfItems - selected;
		else if (selected < 0) selected = numberOfItems + selected;

		oslEndDrawing();
   		oslSyncFrame();

   		sceKernelDelayThread(1000000/__FPS__);
	}

	quit();


	return 0;
}

/**
 * Does what it says, it shuts down the program.
 */
static void quit()
{
	//terminates the program
	oslEndGfx();
	oslQuit();
}

/**
 * Checks so that all the necessary files are in place.
 */
static void initialize(){
	char files[][30] = {"psp.png", "circle.png", "click.wav", "left_test.wav", "right_test.wav"};
	char output[1000] = "";
	bool error = 0;
	SceUID temp;
	int i;

	strcat(output, "PSXChecker version ");
	strcat(output, version);
	strcat(output, " by Xager\n\n");

	for(i = 0; i < sizeof(files) / sizeof(char[30]); i++){
		strcat(output, "Checking for ");
		strcat(output, files[i]);
		strcat(output, "... ");

		temp = sceIoOpen(files[i], IOASSIGN_RDONLY, 0777);
		if(temp < 0){
			error = 1;
			strcat(output, "fail!\n   Make sure that ");
			strcat(output, files[i]);
			strcat(output, " is in the same folder as EBOOT.PBP\n");
		}
		else
			strcat(output, "pass!\n");
	}

	if(error){
		oslStartDrawing();

		oslPrintf("%s\nPress any button to quit...", output);

		oslEndDrawing();
		oslSyncFrame();

		oslWaitKey();
		quit();
	}
}

/**
 * Displays an image of a psp and when the user presses a button
 * a red circle will apear circleing the button.
 */
static void testKeys()
{

	unsigned short analogImageXPos, analogImageYPos;

	//These booleans makes the circles appear the this sections is called.
	//When the user then presses a button that disapears and appears again when the user pressed again.
	bool lon, ron, upon, downon, lefton, righton, holdon, selecton, starton, triangleon, squareon, circleon, crosson;
	lon = ron = upon = downon = lefton = righton = holdon = selecton = starton = triangleon = squareon = circleon = crosson = 1;

	OSL_IMAGE *psp = oslLoadImageFile("psp.png", OSL_IN_RAM, OSL_PF_8888),
		*circle = oslLoadImageFile("circle.png", OSL_IN_RAM, OSL_PF_8888);

	oslInitAudio();
	OSL_SOUND *click = oslLoadSoundFile("click.wav", OSL_FMT_STREAM);


	oslReadKeys();

	while(!osl_quit && (!osl_keys -> held.R || !osl_keys -> held.L))
	{
		oslStartDrawing();

		oslReadKeys();

		oslDrawImage(psp);

		oslSetTextColor(RGB(0,0,0));
		oslSetBkColor(RGBA(0,0,0,0));

		oslPrintf_xy(220, 260, "Press R and L to go back to the menu.");
		oslPrintf_xy(3, 3, "To test the lower buttons press home, then try raising/lowering");
		oslPrintf_xy(3, 13, "the sound with +/-, hold the note to mute and last press");
		oslPrintf_xy(3, 23, "the little screen button and see if it changes brightness.");

		//Each block makes the circle appear.
		if (osl_keys -> held.L || lon)
		{
			oslDrawImageXY(circle, 40, 38);
			if (osl_keys -> held.L && lon) lon = 0;
		}
		if (osl_keys -> held.R || ron)
		{
			oslDrawImageXY(circle, 405, 38);
			if (osl_keys -> held.R && ron) ron = 0;
		}

		if (osl_keys -> held.up || upon)
		{
			oslDrawImageXY(circle, 40, 93);
			if (osl_keys -> held.up && upon) upon = 0;
		}
		if (osl_keys -> held.down || downon)
		{
			oslDrawImageXY(circle, 40, 136);
			if (osl_keys -> held.down && downon) downon = 0;
		}
		if (osl_keys -> held.left || lefton)
		{
			oslDrawImageXY(circle, 18, 115);
			if (osl_keys -> held.left && lefton) lefton = 0;
		}
		if (osl_keys -> held.right || righton)
		{
			oslDrawImageXY(circle, 62, 115);
			if (osl_keys -> held.right && righton) righton = 0;
		}

		if (osl_keys -> held.hold || holdon)
		{
			oslDrawImageXY(circle, 432, 184);
			if (osl_keys -> held.hold && holdon) holdon = 0;
		}
		if (osl_keys -> held.select || selecton)
		{
			oslDrawImageXY(circle, 321, 210);
			if (osl_keys -> held.select && selecton) selecton = 0;
		}
		if (osl_keys -> held.start || starton)
		{
			oslDrawImageXY(circle, 351, 210);
			if (osl_keys -> held.start && starton) starton = 0;
		}

		if (osl_keys -> held.triangle || triangleon)
		{
			oslDrawImageXY(circle, 405, 87);
			if (osl_keys -> held.triangle && triangleon) triangleon = 0;
		}
		if (osl_keys -> held.cross || crosson)
		{
			oslDrawImageXY(circle, 405, 142);
			if (osl_keys -> held.cross && crosson) crosson = 0;
		}
		if (osl_keys -> held.square || squareon)
		{
			oslDrawImageXY(circle, 378, 115);
			if (osl_keys -> held.square && squareon) squareon = 0;
		}
		if (osl_keys -> held.circle || circleon)
		{
			oslDrawImageXY(circle, 432, 115);
			if (osl_keys -> held.circle && circleon) circleon = 0;
		}

		//The reason for not having the sound in the if-statements above is because the circles should be
		//displayed whilst holding the buttons but the sound should only play once
		if (osl_keys->pressed.L || osl_keys->pressed.R || osl_keys->pressed.up || osl_keys->pressed.down ||
			osl_keys->pressed.left || osl_keys->pressed.right || osl_keys->pressed.hold || osl_keys->pressed.select ||
			osl_keys->pressed.start || osl_keys->pressed.triangle || osl_keys->pressed.cross || osl_keys->pressed.square ||
			osl_keys->pressed.circle)
		{
			oslAssert(click);
			oslPlaySound(click, 0);
		}


		analogImageXPos = (unsigned short)floor(10*osl_keys -> analogX/127);
		analogImageYPos = (unsigned short)floor(10*osl_keys -> analogY/127);

		//The joysticks value is also displayed as a numeric value.
		oslPrintf_xy(3, 250, "Joystick horizontal: %d", osl_keys -> analogX);
		oslPrintf_xy(3, 260, "Joystick vertical: %d", osl_keys -> analogY);

		//The reason for not always displaying the joysticks circle is because
		//of it's bad precision, especially with older machines.
		if (abs(osl_keys -> analogX) > __JOYSTICK_MINIMUM__ || abs(osl_keys -> analogY) > __JOYSTICK_MINIMUM__)
			oslDrawImageXY(circle, 42 + analogImageXPos, 177 + analogImageYPos);

		oslEndDrawing();
		oslSyncFrame();

		sceKernelDelayThread(1000000/__FPS__);
	}

	oslDeleteImage(circle);
	oslDeleteImage(psp);
	oslDeleteSound(click);
	oslDeinitAudio();
}

/**
 * Tests the psps sound by playing two tracks with stereo sound, one with
 * only right channel and one with only left.
 */
static void testSound()
{
	oslReadKeys();

	oslInitAudio();

	OSL_SOUND *left = oslLoadSoundFile("left_test.wav", OSL_FMT_STREAM),
		*right = oslLoadSoundFile("right_test.wav", OSL_FMT_STREAM);


	while(!osl_quit && !osl_keys -> held.start)
	{
		oslAudioVSync();

		oslStartDrawing();

		oslCls();

		oslPrintf_xy(60, 120, "Press R for right channel and L for left channel");
		oslPrintf_xy(68, 140, "Press the start button to go back to the menu");

		if (osl_keys -> pressed.L)
		{
			oslAssert(left);
			oslPlaySound(left, 0);
		}
		if (osl_keys -> pressed.R)
		{
			oslAssert(right);
			oslPlaySound(right, 1);
		}

		oslReadKeys();

		oslEndDrawing();
   		oslSyncFrame();

   		sceKernelDelayThread(1000000/__FPS__);
	}

	oslDeleteSound(left);
	oslDeleteSound(right);
	oslDeinitAudio();
}

/**
 * Test the screen for stuck/dead pixels by displaying different colors.
 *
 * Can also display a little square which rapidly changes color.
 */
static void testScreen()
{
	oslStartDrawing();

	oslCls();

	oslPrintf_xy(3, 3, "The screen will now display different colors.");
	oslPrintf_xy(3, 13, "To change color press R for the next color and L for the previous.");
	oslPrintf_xy(3, 33, "To display a \"stuck pixel fixer square\" press X.");
	oslPrintf_xy(3, 43, "Move the square by using the ARROWS and JOYSTICK.");
	oslPrintf_xy(3, 53, "To change the size of the fixer press CIRCLE");
	oslPrintf_xy(3, 73, "Press the START button to return to the main menu.");
	oslPrintf_xy(3, 93, "Press any button when you're ready...");

	oslEndDrawing();
   	oslSyncFrame();

	oslWaitKey();


	bool fixerOn = 0;

	short color = 0;
	short fixerColor = 0;

	unsigned short fixerXCoord = 240, fixerYCoord = 136;

	//0 = 1 pixel, 1 = 3x3 pixels, 2 = 5x5 pixels, 3 = fullscreen
	//The number of pixels should always be odd, 3, 5, 7, etc.
	short fixerSizeSelected = 1;
	short fixerSize = 3;

	//The colors that can be displayed with checking for dead/stuck pixels
	OSL_COLOR colors[] = {RGB(255, 255, 255), RGB(0, 0, 0), RGB(255, 0, 0), RGB(0, 255, 0),
		RGB(0, 0, 255), RGB(255, 255, 0), RGB(255, 0, 255), RGB(0, 255, 255)};


	//The colors that can be displayed when atempting to repair a dead/stuck pixel.
	OSL_COLOR fixerColors[] = {RGB(255, 255, 255), RGB(0, 0, 0), RGB(255, 0, 0), RGB(0, 255, 0), RGB(0, 0, 255)};

	oslReadKeys();

	short numberOfColorItems = sizeof(colors) / sizeof(OSL_COLOR);
	short numberOfFixerColorItems = sizeof(fixerColors) / sizeof(OSL_COLOR);

	while (!osl_quit && !osl_keys -> held.start)
	{
		//~50 FPS, too fast and the fixer will just be white because of the high speed.
		sceKernelDelayThread(1000000/__FPS__);

		oslStartDrawing();

		//The color only changes if the fixer is off or not fullscreen
		if(!fixerOn || fixerSize != 0){
			oslClearScreen(colors[color]);

			if (osl_keys -> pressed.R) color++;
			if (osl_keys -> pressed.L) color--;

			//Makes the colors wrap
			if (color >= numberOfColorItems) color = numberOfColorItems-color;
			else if (color < 0) color = numberOfColorItems+color;
		}

		if (fixerOn)
		{
			if(fixerSize != 0)
			{
				oslDrawFillRect(fixerXCoord-(fixerSize-1)/2, fixerYCoord-(fixerSize-1)/2,
					fixerXCoord+(fixerSize-1)/2+1, fixerYCoord+(fixerSize-1)/2+1, fixerColors[fixerColor]);

				//Makes sure that the pixel is moved but not moved out of the screen.
				if (osl_keys -> pressed.right) fixerXCoord = fixerXCoord+1 >= 479?479:fixerXCoord+1;
				if (osl_keys -> pressed.down) fixerYCoord = fixerYCoord+1 >= 271?271:fixerYCoord+1;
				if (osl_keys -> pressed.left) fixerXCoord = fixerXCoord-1 <= 0?0:fixerXCoord-1;
				if (osl_keys -> pressed.up) fixerYCoord = fixerYCoord-1 <= 0?0:fixerYCoord-1;

				if (osl_keys -> analogX > 64) fixerXCoord = fixerXCoord+5 >= 479?479:fixerXCoord+5;
				if (osl_keys -> analogX < -64) fixerXCoord = fixerXCoord-5 <= 0?0:fixerXCoord-5;
				if (osl_keys -> analogY > 64) fixerYCoord = fixerYCoord+5 >= 271?271:fixerYCoord+5;
				if (osl_keys -> analogY < -64) fixerYCoord = fixerYCoord-5 <= 0?0:fixerYCoord-5;
			}
			else
				oslClearScreen(fixerColors[fixerColor]);

			fixerColor++;
			if (fixerColor >= numberOfFixerColorItems) fixerColor = numberOfFixerColorItems-fixerColor;

			if (osl_keys -> pressed.circle)
			{
				fixerSizeSelected++;
				if (fixerSizeSelected >= 4) fixerSizeSelected = 4-fixerSizeSelected;

				if (fixerSizeSelected == 0) fixerSize = 1;
				else if (fixerSizeSelected == 1) fixerSize = 3;
				else if (fixerSizeSelected == 2) fixerSize = 5;
				else if (fixerSizeSelected == 3) fixerSize = 0;
			}
		}

		if (osl_keys -> pressed.cross) fixerOn = !fixerOn;

		oslReadKeys();

		oslEndDrawing();
		oslWaitVSync();
		oslSyncFrame();
	}
}

/**
 * Displays a short about message
 */
static void displayAbout()
{
	oslStartDrawing();
	oslCls();

	oslPrintf("\n PSXChecker version %s by Xager\n\n"
	" Made for and used by PSXCare (http://www.psxcare.com/)\n"
	" Greetings to everyone at psxcare and pendragon :)\n\n"
	" This program is licensed under GNU General Public License which\n"
	" basicly means that you may use the program and code freely without\n"
	" any charge aslong as the program is open source and free of charge.\n\n"
	" Any questions may be sent to <nicklas.ansman@gmail.com>\n\n"
	" Press any button to return to the main menu...", version);

	oslEndDrawing();
	oslSyncFrame();

	oslWaitKey();
}
