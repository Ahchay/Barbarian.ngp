// copies the new animation onto the Sprites tiles.
void CopySpriteAnimation(u8 SpriteNumber, u16 AnimNumber, u16 SpriteTile)
{

	//Basically, this copies the 32? bytes from the sprite_tile
	//array into the relevant part of TILERAM.
	//It does rather assume that a sprite is 4 tiles in size?

	//Or, at least, that's the theory...
	//In practice, it does tend to overwrite the wrong bit of tileram
	//which explains why I get animated backgrounds all the time...


	//Second beastie copies into TILE_RAM[2]-TILE_RAM[6]?

	u8 i;
	u16 * TileRam = TILE_RAM;
	u16 * SpriteAnimation = (u16 *) Animation;

	TileRam += ((SpriteTile) * 8);
	SpriteAnimation += (AnimNumber * 8);

	for (i = 0; i < 8; i ++)
	{
		TileRam[i] = *SpriteAnimation++;
	}
}

