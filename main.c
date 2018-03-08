#include "ngpc.h"
#include "carthdr.h" // TODO: edit game name in carthdr.h
#include "library.h"
#include "tiles.c"
#include "sprite.c"

#include "barbarian.h"
#include "barbarian.c"

void main()
{
	//Define variables
	//Game element structures
	BARBARIAN bEnemies[MAX_BARBARIANS];
	MISSILE aMissile;
	PLAYER pCatapult;
	ARROW aArrows[MAX_ARROWS];
	HORSEY hBarbarianLeader;

	//General Loop variables
	u8 iBarbarianLoop;
	u8 iBarbarianLoopX;
	u8 iBarbarianLoopY;
	u8 iArrowLoop;
	u8 iLoop;
	u8 iLevel;
	u8 iLoopX;
	u8 iLoopY;
	u16 iArrowTimer;
	u16 iBarbarianTimer;
	u8 iBarbarianCounter;
	u8 iArrowCounter;
	u8 iDeadBarbarianCount;
	u16 iGameTimer;
	u8 iPalette;

	//Setup NGPC System
	InitNGPC();
	SysSetSystemFont();

	//Seed the random number generator...
	SeedRandom;

	//Install the graphics.
	InstallTileSet(BarbarianTile, sizeof(BarbarianTile)/2);

	//Yer basic background colour...
	SetBackgroundColour(RGB(0,0,0));

	//Setup Tile palettes
	SetPalette(SCR_1_PLANE, 0, 0, RGB(15,15,15), RGB(15,15,15), RGB(15,15,15));
	SetPalette(SCR_1_PLANE, 1, 0, 0, 0, 0);
	SetPalette(SCR_2_PLANE, 0, 0, RGB(15,15,15), RGB(15,15,15), RGB(15,15,15));
	SetPalette(SCR_2_PLANE, 1, 0, RGB(2,0,0), RGB(6,0,0), RGB(4,0,0));
	SetPalette(SCR_2_PLANE, 2, 0, RGB(12,12,12), RGB(8,8,8), RGB(4,4,4));

	//Setup Sprite Palettes
	SetPalette(SPRITE_PLANE, 0, 0, RGB(15,0,0), RGB(4,4,4), RGB(15,15,15)); //Invader
	SetPalette(SPRITE_PLANE, 1, 0, RGB(15,0,0), RGB(12,12,12), RGB(0,0,15)); //Pushing Guys
	SetPalette(SPRITE_PLANE, 2, 0, RGB(4,4,4), RGB(12,4,0), RGB(4,0,0)); //Catapult
	SetPalette(SPRITE_PLANE, 3, 0, RGB(15,15,15), RGB(15,15,15), RGB(15,15,15)); //Missile
	SetPalette(SPRITE_PLANE, 4, 0, RGB(15,15,15), RGB(15,15,15), RGB(15,15,15)); //Arrow
	SetPalette(SPRITE_PLANE, 5, 0, RGB(0,15,0), RGB(12,4,0), RGB(15,15,15)); //Archer
	SetPalette(SPRITE_PLANE, 6, 0, RGB(0,0,15), RGB(8,8,8), RGB(15,15,15)); //Knight
	SetPalette(SPRITE_PLANE, 7, 0, RGB(15,7,0), RGB(8,8,15), RGB(15,15,15)); //Knight

	//Draw background & Insert coin message
	ClearScreen(SCR_1_PLANE);
	ClearScreen(SCR_2_PLANE);

	//Draw the background
	for (iLoopX=0;iLoopX<5;iLoopX++)
	{
		for (iLoopY=0;iLoopY<5;iLoopY++)
		{
			//Draw those tiles...
			PutTile(SCR_2_PLANE, 1, (iLoopX*4), (iLoopY*4), TileFloor);
			PutTile(SCR_2_PLANE, 1, (iLoopX*4)+1, (iLoopY*4), TileFloor+1);
			PutTile(SCR_2_PLANE, 1, (iLoopX*4)+2, (iLoopY*4), TileFloor+2);
			PutTile(SCR_2_PLANE, 1, (iLoopX*4)+3, (iLoopY*4), TileFloor+3);
			PutTile(SCR_2_PLANE, 1, (iLoopX*4), (iLoopY*4)+1, TileFloor+4);
			PutTile(SCR_2_PLANE, 1, (iLoopX*4)+1, (iLoopY*4)+1, TileFloor+5);
			PutTile(SCR_2_PLANE, 1, (iLoopX*4)+2, (iLoopY*4)+1, TileFloor+6);
			PutTile(SCR_2_PLANE, 1, (iLoopX*4)+3, (iLoopY*4)+1, TileFloor+7);
			PutTile(SCR_2_PLANE, 1, (iLoopX*4), (iLoopY*4)+2, TileFloor+8);
			PutTile(SCR_2_PLANE, 1, (iLoopX*4)+1, (iLoopY*4)+2, TileFloor+9);
			PutTile(SCR_2_PLANE, 1, (iLoopX*4)+2, (iLoopY*4)+2, TileFloor+10);
			PutTile(SCR_2_PLANE, 1, (iLoopX*4)+3, (iLoopY*4)+2, TileFloor+11);
			PutTile(SCR_2_PLANE, 1, (iLoopX*4), (iLoopY*4)+3, TileFloor+12);
			PutTile(SCR_2_PLANE, 1, (iLoopX*4)+1, (iLoopY*4)+3, TileFloor+13);
			PutTile(SCR_2_PLANE, 1, (iLoopX*4)+2, (iLoopY*4)+3, TileFloor+14);
			PutTile(SCR_2_PLANE, 1, (iLoopX*4)+3, (iLoopY*4)+3, TileFloor+15);
		}
	}
	for (iLoopY=0;iLoopY<3;iLoopY++)
	{
		for (iLoopX=0;iLoopX<=(MAX_X/8);iLoopX++)
		{
			PutTile(SCR_2_PLANE, 2, iLoopX, (MAX_Y/8)-iLoopY, TileWall);
		}
	}

	//Main (forever) loop
	while (1)
	{

		//Setup Player structure
		pCatapult.xPosition=MAX_X/2;
		pCatapult.yPosition=MAX_Y-16;
		pCatapult.Animation=0;
		pCatapult.MaximumHeight=0;
		pCatapult.Direction=0;
		pCatapult.Flags=0;
		pCatapult.Score=0;
		pCatapult.Lives=3;
		pCatapult.PushingGuyAnimation=ANIM_PUSHINGGUYS;
		pCatapult.BaseAnimation=ANIM_PLAYER;
		pCatapult.AnimationInterval=12;
		//Then create the sprite
		SetSprite(SpritePlayer, TilePlayer, 0, pCatapult.xPosition, pCatapult.yPosition, 2, 16);
		SetSprite(SpritePlayer+1, TilePlayer+1, 1, 8, 0, 2, 16);
		SetSprite(SpritePlayer+2, TilePlayer+2, 1, -8, 8, 2, 16);
		SetSprite(SpritePlayer+3, TilePlayer+3, 1, 8, 0, 2, 16);
		SetSprite(SpritePlayer+4, TilePlayer+4, 1, -16, 0, 1, 16);
		SetSprite(SpritePlayer+5, TilePlayer+5, 1, 24, 0, 1, 16);

		//Setup Missile structure & sprite...
		aMissile.xPosition=pCatapult.xPosition+4;
		aMissile.yPosition=pCatapult.yPosition+8;
		aMissile.Animation=0;
		aMissile.Direction=0;
		aMissile.Flags=0;
		aMissile.Height=0;
		aMissile.Strength=0;
		aMissile.BaseAnimation=ANIM_MISSILE;
		aMissile.AnimationInterval=8;
		SetSprite(SpriteMissile, TileMissile, 0, aMissile.xPosition, aMissile.yPosition, 3, 16);

		iLevel=0;

		//Wait for start of game
		PrintString(SCR_1_PLANE, 0, 4, 3, "GAME  OVER");

		PrintString(SCR_1_PLANE, 0, 1, 7, "Press A Button to");
		PrintString(SCR_1_PLANE, 0, 3, 8, "play Barbarian");
		PrintString(SCR_1_PLANE, 0, 6, 9, "Invaders");
		//Wait for the "A" button
		while (!(JOYPAD & J_A));

		ClearScreen(SCR_1_PLANE);

		//Game loop
		while (pCatapult.Lives>0)
		{
			//Setup start of level/next life variables
			if (pCatapult.Flags==1)
			{
				pCatapult.Lives--;
			}
			else
			{
				//Setup the new level...
				iDeadBarbarianCount=0;
				//Create the barbarians
				iBarbarianTimer=16;
				iArrowTimer=24;

				for(iBarbarianLoopX=0;iBarbarianLoopX<(MAX_BARBARIANS/5);iBarbarianLoopX++)
				{
					for (iBarbarianLoopY=0;iBarbarianLoopY<5;iBarbarianLoopY++)
					{
						//Create the barbarian structure
						bEnemies[((iBarbarianLoopX * 5)+iBarbarianLoopY)].xPosition=96 - (iBarbarianLoopX * 12);
						bEnemies[((iBarbarianLoopX * 5)+iBarbarianLoopY)].yPosition=(16 * iBarbarianLoopY)+16+(8*(iLevel % 5));
						bEnemies[((iBarbarianLoopX * 5)+iBarbarianLoopY)].Animation=0;
						bEnemies[((iBarbarianLoopX * 5)+iBarbarianLoopY)].Direction=DirRight;
						bEnemies[((iBarbarianLoopX * 5)+iBarbarianLoopY)].Flags=0;
						if (iBarbarianLoopY==0)
						{
							bEnemies[((iBarbarianLoopX * 5)+iBarbarianLoopY)].BaseAnimation=ANIM_BARBARIANKNIGHT;
							iPalette=6;
						}
						else if (iBarbarianLoopY<=2)
						{
							bEnemies[((iBarbarianLoopX * 5)+iBarbarianLoopY)].BaseAnimation=ANIM_BARBARIANARCHER;
							iPalette=5;
						}
						else
						{
							bEnemies[((iBarbarianLoopX * 5)+iBarbarianLoopY)].BaseAnimation=ANIM_BARBARIANGRUNT;
							iPalette=0;
						}
						//Create the sprite
						CopySpriteAnimation(SpriteBarbarian+((iBarbarianLoopX * 5)+iBarbarianLoopY), bEnemies[((iBarbarianLoopX * 5)+iBarbarianLoopY)].BaseAnimation, TileBarbarian + ((iBarbarianLoopX * 5)+iBarbarianLoopY));
						SetSprite(SpriteBarbarian+((iBarbarianLoopX * 5)+iBarbarianLoopY), TileBarbarian + ((iBarbarianLoopX * 5)+iBarbarianLoopY), 0, bEnemies[((iBarbarianLoopX * 5)+iBarbarianLoopY)].xPosition, bEnemies[((iBarbarianLoopX * 5)+iBarbarianLoopY)].yPosition, iPalette, 16);
					}
				}
				//Create the barbarian Leader
				hBarbarianLeader.xPosition=0;
				hBarbarianLeader.yPosition=0;
				hBarbarianLeader.Animation=0;
				hBarbarianLeader.Direction=DirRight;
				hBarbarianLeader.BaseAnimation=ANIM_HORSEY;
				hBarbarianLeader.Flags=0;
				hBarbarianLeader.AnimationInterval=16000;
				//Create the sprite
				CopySpriteAnimation(SpriteHorsey, ANIM_NULL, TileHorsey);
				CopySpriteAnimation(SpriteHorsey+1, ANIM_NULL, TileHorsey+1);
				CopySpriteAnimation(SpriteHorsey+2, ANIM_NULL, TileHorsey+2);
				CopySpriteAnimation(SpriteHorsey+3, ANIM_NULL, TileHorsey+3);
				SetSprite(SpriteHorsey, TileHorsey, 0,  hBarbarianLeader.xPosition, hBarbarianLeader.yPosition, 7, 16);
				SetSprite(SpriteHorsey+1, TileHorsey+1, 1, 8, 0, 0, 16);
				SetSprite(SpriteHorsey+2, TileHorsey+2, 1, -8, 8, 0, 16);
				SetSprite(SpriteHorsey+3, TileHorsey+3, 1, 8, 0, 0, 16);
				iLevel++;

			}

			PrintString(SCR_1_PLANE, 0, 10, 18, "      ");
			for (iLoop=0;iLoop<pCatapult.Lives;iLoop++)
			{
				PutTile(SCR_1_PLANE, 1, 10+iLoop, 18, TilePlayer+4);
			}
			//Setup Arrow sprites
			for (iArrowLoop=0;iArrowLoop<MAX_ARROWS;iArrowLoop++)
			{
				aArrows[iArrowLoop].xPosition=0;
				aArrows[iArrowLoop].yPosition=0;
				aArrows[iArrowLoop].Animation=0;
				aArrows[iArrowLoop].Flags=1;

				aArrows[iArrowLoop].BaseAnimation=ANIM_ARROW;
				//Create the sprite
				SetSprite(SpriteArrow+iArrowLoop, TileArrow+iArrowLoop, 0, aArrows[iArrowLoop].xPosition, aArrows[iArrowLoop].yPosition, 4, 16);
				CopySpriteAnimation(SpriteArrow+iArrowLoop, ANIM_NULL, TileArrow+iArrowLoop);
			}
			//Reset the player "alive" flag
			pCatapult.Flags=0;

			if (pCatapult.Lives>0)
			{
				//Give the player some warning...
				PrintString(SCR_1_PLANE, 0, 7, 10, "Ready!");
				Sleep(100);
				PrintString(SCR_1_PLANE, 0, 7, 10, "      ");
			}

			//Life/Level Loop
			iGameTimer=0;
			iBarbarianCounter=0;
			while ((pCatapult.Flags==0) && (iDeadBarbarianCount<MAX_BARBARIANS) && (pCatapult.Lives>0))
			{
				//Animate Enemies
				if (((iGameTimer % iBarbarianTimer)==0)&&(iDeadBarbarianCount<MAX_BARBARIANS))
				{
					while (bEnemies[iBarbarianCounter].Flags!=0)
					{
						iBarbarianCounter++;
						if (iBarbarianCounter>=MAX_BARBARIANS)
							iBarbarianCounter=0;
					}
					bEnemies[iBarbarianCounter].Animation ^= 1;
					if (bEnemies[iBarbarianCounter].Direction==DirLeft)
					{
						bEnemies[iBarbarianCounter].xPosition--;
						if (bEnemies[iBarbarianCounter].xPosition==0)
						{
							bEnemies[iBarbarianCounter].yPosition+=8;
							bEnemies[iBarbarianCounter].Direction=DirRight;
						}
					}
					else
					{
						bEnemies[iBarbarianCounter].xPosition++;
						if (bEnemies[iBarbarianCounter].xPosition==MAX_X-8)
						{
							bEnemies[iBarbarianCounter].yPosition+=8;
							bEnemies[iBarbarianCounter].Direction=DirLeft;
							if (bEnemies[iBarbarianCounter].yPosition>=MAX_Y-16)
							{
								//The barbarians have won. Instant game over...
								pCatapult.Flags=1;
								pCatapult.Lives=1;
							}
						}
					}
					CopySpriteAnimation(SpriteBarbarian+iBarbarianCounter, bEnemies[iBarbarianCounter].BaseAnimation+bEnemies[iBarbarianCounter].Animation, TileBarbarian+iBarbarianCounter);
					SetSpritePosition(SpriteBarbarian+iBarbarianCounter, bEnemies[iBarbarianCounter].xPosition, bEnemies[iBarbarianCounter].yPosition);

					//The barbarian has a $foo% chance of launching an arrow at the player...
					if (GetRandom(100)<5)
					{
						for (iArrowLoop=0;iArrowLoop<MAX_ARROWS;iArrowLoop++)
						{
							if (aArrows[iArrowLoop].Flags==1)
							{
								//Create an arrow
								aArrows[iArrowLoop].Flags=0;
								aArrows[iArrowLoop].xPosition=bEnemies[iBarbarianCounter].xPosition;
								aArrows[iArrowLoop].yPosition=bEnemies[iBarbarianCounter].yPosition;
								//Exit this loop
								iArrowLoop=MAX_ARROWS;
							}
						}
					}

					iBarbarianCounter++;
					if (iBarbarianCounter>=MAX_BARBARIANS)
						iBarbarianCounter=0;
				}

				//Barbarian Leader
				if ((iGameTimer % hBarbarianLeader.AnimationInterval)==0)
				{
					if (hBarbarianLeader.Flags==0)
					{
						//Move the leader...
						hBarbarianLeader.xPosition++;
						hBarbarianLeader.Animation^=1;
						CopySpriteAnimation(SpriteHorsey, hBarbarianLeader.BaseAnimation+(hBarbarianLeader.Animation*4), TileHorsey);
						CopySpriteAnimation(SpriteHorsey+1, hBarbarianLeader.BaseAnimation+(hBarbarianLeader.Animation*4)+1, TileHorsey+1);
						CopySpriteAnimation(SpriteHorsey+2, hBarbarianLeader.BaseAnimation+(hBarbarianLeader.Animation*4)+2, TileHorsey+2);
						CopySpriteAnimation(SpriteHorsey+3, hBarbarianLeader.BaseAnimation+(hBarbarianLeader.Animation*4)+3, TileHorsey+3);
						SetSpritePosition(SpriteHorsey, hBarbarianLeader.xPosition, hBarbarianLeader.yPosition);

						if (hBarbarianLeader.xPosition<MAX_X)
						{
							hBarbarianLeader.AnimationInterval=iBarbarianTimer;
						}
						else
						{
							hBarbarianLeader.AnimationInterval=16000;
							hBarbarianLeader.xPosition=0;
							hBarbarianLeader.Flags=1;
							//Create the sprite
							CopySpriteAnimation(SpriteHorsey, ANIM_NULL, TileHorsey);
							CopySpriteAnimation(SpriteHorsey+1, ANIM_NULL, TileHorsey+1);
							CopySpriteAnimation(SpriteHorsey+2, ANIM_NULL, TileHorsey+2);
							CopySpriteAnimation(SpriteHorsey+3, ANIM_NULL, TileHorsey+3);
						}
					}
					else
					{
						hBarbarianLeader.Flags=0;
						hBarbarianLeader.AnimationInterval=16000;
						hBarbarianLeader.xPosition=0;
						//Create the sprite
						CopySpriteAnimation(SpriteHorsey, ANIM_NULL, TileHorsey);
						CopySpriteAnimation(SpriteHorsey+1, ANIM_NULL, TileHorsey+1);
						CopySpriteAnimation(SpriteHorsey+2, ANIM_NULL, TileHorsey+2);
						CopySpriteAnimation(SpriteHorsey+3, ANIM_NULL, TileHorsey+3);
					}
				}

				//Animate Arrows
				if ((iGameTimer % iArrowTimer)==0)
				{
					for (iArrowLoop=0;iArrowLoop<MAX_ARROWS;iArrowLoop++)
					{
						if (aArrows[iArrowLoop].Flags==0)
						{
							//Update the arrow position and other flags
							aArrows[iArrowLoop].yPosition++;
							//aArrows[iArrowLoop].Animation^=1;

							//Copy the sprite
							CopySpriteAnimation(SpriteArrow+iArrowLoop, aArrows[iArrowLoop].BaseAnimation+aArrows[iArrowLoop].Animation, TileArrow+iArrowLoop);
							SetSpritePosition(SpriteArrow+iArrowLoop, aArrows[iArrowLoop].xPosition, aArrows[iArrowLoop].yPosition);

							//Check to see whether we've hit anything (the ground for now)
							if ((aArrows[iArrowLoop].yPosition+8>=pCatapult.yPosition+8) && (aArrows[iArrowLoop].yPosition+8<=pCatapult.yPosition+16) && (aArrows[iArrowLoop].xPosition+4>=pCatapult.xPosition-8) && (aArrows[iArrowLoop].xPosition+4<=pCatapult.xPosition))
							{
								//Hit!
								//Left guy
								pCatapult.Flags=1;
								CopySpriteAnimation(SpritePlayer+4, ANIM_DEADGOODGUY, TilePlayer+4);
							} else if ((aArrows[iArrowLoop].yPosition+8>=pCatapult.yPosition+8) && (aArrows[iArrowLoop].yPosition+8<=pCatapult.yPosition+16) && (aArrows[iArrowLoop].xPosition+4>=pCatapult.xPosition+16) && (aArrows[iArrowLoop].xPosition+4<=pCatapult.xPosition+24))
							{
								//Hit!
								//Right guy
								pCatapult.Flags=1;
								CopySpriteAnimation(SpritePlayer+5, ANIM_DEADGOODGUY, TilePlayer+5);
							} else if (aArrows[iArrowLoop].yPosition==MAX_Y)
								aArrows[iArrowLoop].Flags=1;
						}
						else
						{
							//Make sure the sprite is invisible...
							CopySpriteAnimation(SpriteArrow+iArrowLoop, ANIM_NULL, TileArrow+iArrowLoop);
						}
					}
				}

				//Animate Missiles
				if ((iGameTimer % aMissile.AnimationInterval)==0)
				{
					if (aMissile.Flags == 0)
					{
						//Missile is tied to the catapult.
						aMissile.xPosition=pCatapult.xPosition+4;
						aMissile.yPosition=pCatapult.yPosition+5+(pCatapult.MaximumHeight*2);
					}
					else
					{
						//Missile has been fired
						aMissile.yPosition=aMissile.yPosition-1;

						if (aMissile.Direction==0)
						{
							//Going up...
							aMissile.Height++;
							aMissile.Animation=(aMissile.Height/((aMissile.Strength/4)+1));
							if (aMissile.Height==aMissile.Strength)
								aMissile.Direction=1;
						}
						else
						{
							//Coming down
							aMissile.Height--;
							aMissile.Animation=(aMissile.Height/((aMissile.Strength/4)+1));
							if (aMissile.Height==0)
							{
								//Check to see whether we've hit a barbarian or not...
								for (iBarbarianLoop=0;iBarbarianLoop<MAX_BARBARIANS;iBarbarianLoop++)
								{
									if (((aMissile.xPosition+4)>=bEnemies[iBarbarianLoop].xPosition) && ((aMissile.xPosition+4)<=(bEnemies[iBarbarianLoop].xPosition+8)) && ((aMissile.yPosition+4)>=bEnemies[iBarbarianLoop].yPosition) && ((aMissile.yPosition+4)<=(bEnemies[iBarbarianLoop].yPosition+8)) && (bEnemies[iBarbarianLoop].Flags==0))
									{
										iDeadBarbarianCount++;
										pCatapult.Score+=(aMissile.Strength*10);
										bEnemies[iBarbarianLoop].Flags=1;
										CopySpriteAnimation(SpriteBarbarian+iBarbarianLoop, ANIM_DEADGUY, TileBarbarian+iBarbarianLoop);
									}
								}
								//Check to see whether we've hit the barbarian leader
								//Reset the missile.
								if (((aMissile.xPosition+4)>=hBarbarianLeader.xPosition) && ((aMissile.xPosition+4)<=(hBarbarianLeader.xPosition+8)) && ((aMissile.yPosition+4)>=hBarbarianLeader.yPosition) && ((aMissile.yPosition+4)<=(hBarbarianLeader.yPosition+8)) && (hBarbarianLeader.Flags==0))
								{
									pCatapult.Score+=(500);
									hBarbarianLeader.Flags=1;
									hBarbarianLeader.AnimationInterval+=5000;
									CopySpriteAnimation(SpriteHorsey, ANIM_DEADHORSEY, TileHorsey);
									CopySpriteAnimation(SpriteHorsey+1, ANIM_DEADHORSEY+1, TileHorsey+1);
									CopySpriteAnimation(SpriteHorsey+2, ANIM_DEADHORSEY+2, TileHorsey+2);
									CopySpriteAnimation(SpriteHorsey+3, ANIM_DEADHORSEY+3, TileHorsey+3);
								}
								aMissile.Flags=0;
								aMissile.Direction=0;
								aMissile.Strength=0;
								pCatapult.MaximumHeight=0;
							}
						}
						CopySpriteAnimation(SpriteMissile, aMissile.BaseAnimation+aMissile.Animation, TileMissile);
					}
					SetSpritePosition(SpriteMissile, aMissile.xPosition, aMissile.yPosition);
				}

				//Animate Player
				if ((iGameTimer % pCatapult.AnimationInterval)==0)
				{
					if (pCatapult.Flags==0)
					{
						if (JOYPAD & J_LEFT)
						{
							if (pCatapult.xPosition>0)
							{
								pCatapult.Animation ^=1;
								pCatapult.xPosition--;
							}
						} else if (JOYPAD & J_RIGHT)
						{
							if (pCatapult.xPosition<MAX_X-16)
							{
								pCatapult.xPosition++;
								pCatapult.Animation ^=1;
							}
						}
						if (JOYPAD & J_A)
						{
							//Fire!
							if (aMissile.Flags==0)
							{
								//This really needs to update the player animation thang to
								//give some indication of how "wound" the catapult is...
								if (aMissile.Strength < 72)
									pCatapult.MaximumHeight=aMissile.Strength/18;
									aMissile.Strength++;
							}
						}
						else
						{
							if (aMissile.Flags==0)
							{
								if (aMissile.Strength>0)
								{
									aMissile.Flags=1;
									aMissile.Direction=0;
									pCatapult.MaximumHeight=0;
								}
							}
						}
						CopySpriteAnimation(SpritePlayer, pCatapult.BaseAnimation+(pCatapult.MaximumHeight*4), TilePlayer);
						CopySpriteAnimation(SpritePlayer+1, pCatapult.BaseAnimation+(pCatapult.MaximumHeight*4)+1, TilePlayer+1);
						CopySpriteAnimation(SpritePlayer+2, pCatapult.BaseAnimation+(pCatapult.MaximumHeight*4)+2, TilePlayer+2);
						CopySpriteAnimation(SpritePlayer+3, pCatapult.BaseAnimation+(pCatapult.MaximumHeight*4)+3, TilePlayer+3);
						CopySpriteAnimation(SpritePlayer+4, pCatapult.PushingGuyAnimation+(pCatapult.Animation*2), TilePlayer+4);
						CopySpriteAnimation(SpritePlayer+5, pCatapult.PushingGuyAnimation+(pCatapult.Animation*2)+1, TilePlayer+5);
						SetSpritePosition(SpritePlayer, pCatapult.xPosition, pCatapult.yPosition);
					}
				}

				PrintDecimal(SCR_1_PLANE, 1, 0, 18, pCatapult.Score, 8);

				iGameTimer++;
			}
		}
	}

}


