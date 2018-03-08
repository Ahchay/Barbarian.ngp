#define MAX_X 152
#define MAX_Y 144
#define MAX_BARBARIANS 40
#define MAX_ARROWS 8

#define DirLeft 1
#define DirRight 2

typedef struct Barbarian
{
	u8 xPosition;
	u8 yPosition;
	u8 Animation;
	u8 Direction;
	u8 Flags;
	u8 BaseAnimation;
	u16 AnimationInterval;
} BARBARIAN;

typedef struct Player
{
	u8 xPosition;
	u8 yPosition;
	u8 Animation;
	u8 MaximumHeight;
	u8 Direction;
	u8 Flags;
	u8 Lives;
	u16 Score;
	u8 BaseAnimation;
	u8 PushingGuyAnimation;
	u16 AnimationInterval;
} PLAYER;

typedef struct Missile
{
	u8 xPosition;
	u8 yPosition;
	u8 Animation;
	u8 Direction;
	u8 Flags;
	u8 Height;
	u8 BaseAnimation;
	u8 Strength;
	u16 AnimationInterval;
} MISSILE;

typedef struct Arrow
{
	u8 xPosition;
	u8 yPosition;
	u8 Animation;
	u8 Direction;
	u8 Flags;
	u8 BaseAnimation;
} ARROW;

typedef struct Horsey
{
	u8 xPosition;
	u8 yPosition;
	u8 Animation;
	u8 Direction;
	u8 Flags;
	u8 BaseAnimation;
	u16 AnimationInterval;
} HORSEY;