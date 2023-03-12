#include "IO.h" 

int g_Cursor=0;

void SetCursor(int X)
{
	WritePort_Out(0x3D4,0x0E);
	WritePort_Out(0x3D5,X>>8);
	
	WritePort_Out(0x3D4,0x0F);
	WritePort_Out(0x3D5,X&0xFF);
}

void Scroll(void)
{
	int i,j;
	char *Video_Mem=(char*)VIDEO;
	for(i=0; i<HEIGHT-1; i++)
	{
		for(j=0; j<WIDTH*2; j++)
		{
			Video_Mem[i*WIDTH*2+j]=Video_Mem[(i+1)*WIDTH*2+j];
		}
	}
	Video_Mem=Video_Mem+(HEIGHT-1)*WIDTH*2;
	for(i=0; i<WIDTH*2; i+=2)
	{
		Video_Mem[i]=' ';
		Video_Mem[i+1]=DEFAULT_COLOR;
	}
	g_Cursor=g_Cursor-WIDTH;
}

void putchar(const char _Format)
{
	char *Video_Mem=(char*)VIDEO+g_Cursor*2;
	switch(_Format)
	{
		case '\n':
			LineFeed(&Video_Mem);
			break;
			
		case KEY_BACKSPACE:
			BackSpace(&Video_Mem);
			break;
				
		default:
			Video_Mem[0]=_Format; Video_Mem[1]=DEFAULT_COLOR;
			if(CURSOR_LIMIT<=g_Cursor)
			{
				Scroll();
			}
			g_Cursor++;
			break;
	}

	SetCursor(g_Cursor);
}

void LineFeed(char **Video_Mem)
{
	int Feed_=WIDTH*2-(((long long)*Video_Mem-VIDEO)%(WIDTH*2));
	*Video_Mem=*Video_Mem+Feed_;
	g_Cursor=g_Cursor+Feed_/2;
	if(CURSOR_LIMIT<g_Cursor)
		Scroll();
}

void BackSpace(char **Video_Mem)
{
	if(*Video_Mem>(char *)VIDEO&&*(*Video_Mem-1)!=PATH_COLOR)
	{
		*(*Video_Mem-1)=DEFAULT_COLOR;
		*(*Video_Mem-2)=' ';
		*Video_Mem=(*Video_Mem-2);
		g_Cursor--;
	}
}

void SetColor_Path(int Path_Length)
{
	int i;
	char *Video_Mem=(char*)VIDEO+g_Cursor*2;
	for(i=0; i<Path_Length*2; i+=2)
		*(Video_Mem-i-1)=PATH_COLOR;
}

int printf(const char * _Format,...)
{
	int _EOF;
	int i,len_String=0;
	va_list pParam;
	va_start(pParam,_Format);
	
	char *Video_Mem=(char*)VIDEO+g_Cursor*2;
	for(i=0; _Format[i]!='\0'; i++)
	{
		switch(_Format[i])
		{
			case '\n':
				LineFeed(&Video_Mem);
				break;
				
			case '%':
				_EOF=Arg_printf(_Format[++i],pParam,&Video_Mem);
				if(_EOF==EOF)
					return EOF;
				break;
			default:
				Video_Mem[0]=_Format[i];
				Video_Mem[1]=DEFAULT_COLOR;
				Video_Mem+=2;
				if(CURSOR_LIMIT<=g_Cursor)
				{
					Scroll();
					Video_Mem=Video_Mem-WIDTH*2;
				}
				g_Cursor++;
				break;
		}
	}
	va_end(pParam);
	SetCursor(g_Cursor);
}

int Arg_printf(char Arg_Type,va_list Arg_,char **Video_Mem)
{
	int _EOF;
	int len_String=0;
	switch(Arg_Type)
	{
		case 'd':
		case 'i':
			Num_to_String((long long int)(va_arg(Arg_,long long int)),Video_Mem);
			break;
		case 'c':
			(*Video_Mem)[0]=(char)(va_arg(Arg_,int));
			(*Video_Mem)[1]=DEFAULT_COLOR;
			(*Video_Mem)+=2;
			if(CURSOR_LIMIT<=g_Cursor)
			{
				Scroll();
				Video_Mem=Video_Mem-WIDTH*2;
			}
			g_Cursor++;
			break;
		case 's':
			_EOF=PrintString((char *)(va_arg(Arg_,char *)),Video_Mem);
			if(_EOF==EOF)
				return EOF;
			break;
		default:
			break;
	}
	return 0;
}


int PrintString(const char* Message,char **Video_Mem)
{
	int i;
	for(i=0; Message[i]!='\0'; i++)
	{
		if(Message[i]==EOF)
			return EOF;
		(*Video_Mem)[0]=Message[i];
		(*Video_Mem)[1]=DEFAULT_COLOR;
		(*Video_Mem)+=2;
		if(CURSOR_LIMIT<=g_Cursor)
		{
			Scroll();
			*Video_Mem=*Video_Mem-WIDTH*2;
		}
		g_Cursor++;
	}
	return 0;
}

void Num_to_String(long long int Number,char **Video_Mem)
{
	long long int Temp=Number;
	if(Number<0)
		Number*=-1;
	char Array[100];
	int i,Count=0;
	while(Number>=10)
	{
		Array[Count]=Number%10+48;
		Number/=10;
		Count++;
	}
	Array[Count]=Number%10+48;
	if(Temp<0)
		Array[++Count]='-';
	for(i=Count; i>=0; i--)
	{
		(*Video_Mem)[0]=Array[i];
		(*Video_Mem)[1]=DEFAULT_COLOR;
		(*Video_Mem)+=2;
		if(CURSOR_LIMIT<=g_Cursor)
		{
			Scroll();
			*Video_Mem=*Video_Mem-WIDTH*2;
		}
		g_Cursor++;
	}
}

void asd(int Number,int Offset)
{
	char* Video=(char*)VIDEO+(20*WIDTH+Offset)*2;
	char Array[100];
	int Count=0;
	while(Number>=10)
	{
		Array[Count]=Number%10;
		Number/=10;
		Count++;
	}
	Array[Count]=Number%10;

	while(Count>=0)
	{
		*Video=Array[Count--]+48;
		Video++;
		*Video=_BLUE;
		Video++;
	}
}

void printk(const char *Message,int line,int Offset,uint Color)
{
	int i;
	char* Video=(char*)VIDEO+(line*WIDTH+Offset)*2;
	
	for(i=0; Message[i]!='\0'; i++)
	{
		*Video=Message[i];
		Video++;
		*Video=Color;
		Video++;
	}
	return ;
}

void cleark(void)
{
	int i;
	char* Video=(char*)VIDEO;
	
	for(i=0; i<WIDTH*HEIGHT; i++)
	{
		*Video=' ';
		Video++;
		*Video=DEFAULT_COLOR;
		Video++;
	}
}

static int PutIndex=0,GetIndex=0,Buf_Count=0;
static unsigned char Key_Buf[BUFFER_SIZE];

unsigned char GetKeyBuf(unsigned char* _Flag)
{
	unsigned char KEY;
	if(Buf_Count<=0)
	{
		*_Flag=false;
		return 0;
	}
	KEY=Key_Buf[GetIndex];
	Buf_Count--;
	GetIndex=(GetIndex+1)%BUFFER_SIZE;
	*_Flag=true;
	return KEY;
}

unsigned char PutKeyBuf(unsigned char KEY)
{
	if(Buf_Count>=BUFFER_SIZE)
		return false;
	Key_Buf[PutIndex]=KEY;
	PutIndex=(PutIndex+1)%BUFFER_SIZE;
	Buf_Count++;
	return true;
}

static bool Key_Manager[3]={true,false,false};
static bool Shift_Down=false;
static KeyTable _Key_Table[89]= {
 	/*  0   */  {   KEY_NONE        ,   KEY_NONE        },
    /*  1   */  {   KEY_ESC         ,   KEY_ESC         },
    /*  2   */  {   '1'             ,   '!'             },
    /*  3   */  {   '2'             ,   '@'             },
    /*  4   */  {   '3'             ,   '#'             },
    /*  5   */  {   '4'             ,   '$'             },
    /*  6   */  {   '5'             ,   '%'             },
    /*  7   */  {   '6'             ,   '^'             },
    /*  8   */  {   '7'             ,   '&'             },
    /*  9   */  {   '8'             ,   '*'             },
    /*  10  */  {   '9'             ,   '('             },
    /*  11  */  {   '0'             ,   ')'             },
    /*  12  */  {   '-'             ,   '_'             },
    /*  13  */  {   '='             ,   '+'             },
    /*  14  */  {   KEY_BACKSPACE   ,   KEY_BACKSPACE   },
    /*  15  */  {   KEY_TAB         ,   KEY_TAB         },
    /*  16  */  {   'q'             ,   'Q'             },
    /*  17  */  {   'w'             ,   'W'             },
    /*  18  */  {   'e'             ,   'E'             },
    /*  19  */  {   'r'             ,   'R'             },
    /*  20  */  {   't'             ,   'T'             },
    /*  21  */  {   'y'             ,   'Y'             },
    /*  22  */  {   'u'             ,   'U'             },
    /*  23  */  {   'i'             ,   'I'             },
    /*  24  */  {   'o'             ,   'O'             },
    /*  25  */  {   'p'             ,   'P'             },
    /*  26  */  {   '['             ,   '{'             },
    /*  27  */  {   ']'             ,   '}'             },
    /*  28  */  {   '\n'            ,   '\n'            },
    /*  29  */  {   KEY_CTRL        ,   KEY_CTRL        },
    /*  30  */  {   'a'             ,   'A'             },
    /*  31  */  {   's'             ,   'S'             },
    /*  32  */  {   'd'             ,   'D'             },
    /*  33  */  {   'f'             ,   'F'             },
    /*  34  */  {   'g'             ,   'G'             },
    /*  35  */  {   'h'             ,   'H'             },
    /*  36  */  {   'j'             ,   'J'             },
    /*  37  */  {   'k'             ,   'K'             },
    /*  38  */  {   'l'             ,   'L'             },
    /*  39  */  {   ';'             ,   ':'             },
    /*  40  */  {   '\''            ,   '\"'            },
    /*  41  */  {   '`'             ,   '~'             },
    /*  42  */  {   KEY_LSHIFT      ,   KEY_LSHIFT      },
    /*  43  */  {   '\\'            ,   '|'             },
    /*  44  */  {   'z'             ,   'Z'             },
    /*  45  */  {   'x'             ,   'X'             },
    /*  46  */  {   'c'             ,   'C'             },
    /*  47  */  {   'v'             ,   'V'             },
    /*  48  */  {   'b'             ,   'B'             },
    /*  49  */  {   'n'             ,   'N'             },
    /*  50  */  {   'm'             ,   'M'             },
    /*  51  */  {   ','             ,   '<'             },
    /*  52  */  {   '.'             ,   '>'             },
    /*  53  */  {   '/'             ,   '?'             },
    /*  54  */  {   KEY_RSHIFT      ,   KEY_RSHIFT      },
    /*  55  */  {   '*'             ,   '*'             },
    /*  56  */  {   KEY_LALT        ,   KEY_LALT        },
    /*  57  */  {   ' '             ,   ' '             },
    /*  58  */  {   KEY_CAPSLOCK    ,   KEY_CAPSLOCK    },
    /*  59  */  {   KEY_F1          ,   KEY_F1          },
    /*  60  */  {   KEY_F2          ,   KEY_F2          },
    /*  61  */  {   KEY_F3          ,   KEY_F3          },
    /*  62  */  {   KEY_F4          ,   KEY_F4          },
    /*  63  */  {   KEY_F5          ,   KEY_F5          },
    /*  64  */  {   KEY_F6          ,   KEY_F6          },
    /*  65  */  {   KEY_F7          ,   KEY_F7          },
    /*  66  */  {   KEY_F8          ,   KEY_F8          },
    /*  67  */  {   KEY_F9          ,   KEY_F9          },
    /*  68  */  {   KEY_F10         ,   KEY_F10         },
    /*  69  */  {   KEY_NUMLOCK     ,   KEY_NUMLOCK     },
    /*  70  */  {   KEY_SCROLLLOCK  ,   KEY_SCROLLLOCK  },
    /*  71  */  {   KEY_HOME        ,   '7'             },
    /*  72  */  {   KEY_UP          ,   '8'             },
    /*  73  */  {   KEY_PAGEUP      ,   '9'             },
    /*  74  */  {   '-'             ,   '-'             },
    /*  75  */  {   KEY_LEFT        ,   '4'             },
    /*  76  */  {   KEY_CENTER      ,   '5'             },
    /*  77  */  {   KEY_RIGHT       ,   '6'             },
    /*  78  */  {   '+'             ,   '+'             },
    /*  79  */  {   KEY_END         ,   '1'             },
    /*  80  */  {   KEY_DOWN        ,   '2'             },
    /*  81  */  {   KEY_PAGEDOWN    ,   '3'             },
    /*  82  */  {   KEY_INS         ,   '0'             },
    /*  83  */  {   KEY_DEL         ,   '.'             },
    /*  84  */  {   KEY_NONE        ,   KEY_NONE        },
    /*  85  */  {   KEY_NONE        ,   KEY_NONE        },
    /*  86  */  {   KEY_NONE        ,   KEY_NONE        },
    /*  87  */  {   KEY_F11         ,   KEY_F11         },
    /*  88  */  {   KEY_F12         ,   KEY_F12         }
};

bool Is_Ack(void)
{
	int i;
	for(i=0; i<10000; i++)
	{
		if(ReadPort_In(0x60)==ACK)
			return true;
	}
	return false;
}

bool IsBuf_full(uint In_Out)
{
	if(ReadPort_In(0x64)&In_Out)
		return true;
	return false;
}

bool EnableKeyBoard(void)
{
	unsigned long long int RFLAGS;
	bool Return;
	RFLAGS=Read_Rflags();
	__asm__ __volatile__("cli;");
	WritePort_Out(0x64,0xAE);
	while(IsBuf_full(In_Buf));
	
	WritePort_Out(0x60,0xF4);
	while(!IsBuf_full(Out_Buf));
	
	Return=Is_Ack();
	if(RFLAGS&0x200)
			__asm__ __volatile("sti;");
	return Return;
}

bool On_Off_LED(uchar Num_Lock,uchar Caps_Lock,uchar Scroll_Lock)
{
	unsigned long long int RFLAGS;
	bool Return;
	while(IsBuf_full(In_Buf));
	WritePort_Out(0x60,0xED);
	
	while(IsBuf_full(In_Buf));
	RFLAGS=Read_Rflags();
	__asm__ __volatile__("cli;");
	if(!Is_Ack())
	{
		if(RFLAGS&0x200)
			__asm__ __volatile("sti;");
		return false;
	}
		
	WritePort_Out(0x60,(Num_Lock<<1)|(Caps_Lock<<2)|(Scroll_Lock));
	while(IsBuf_full(In_Buf));
	
	Return=Is_Ack();
	if(RFLAGS&0x200)
			__asm__ __volatile("sti;");
	return Return;
}

bool Scan_Convert_ASCII(uchar ScanCode,uchar *ASCII_Code)
{
	bool _Flag;
	if(ScanCode==0x2A||ScanCode==0x36)
	{
		Shift_Down=true;
		*ASCII_Code=_Key_Table[ScanCode].NormalKey;
		_Flag=true;
		return _Flag;
	}
	else if(ScanCode==0xAA||ScanCode==0xB6)
	{
		Shift_Down=false;
		*ASCII_Code=_Key_Table[ScanCode].NormalKey;
		_Flag=false;
		return _Flag;
	}
	else if(ScanCode==0x45) //NumLock
	{
		Key_Manager[0]=Key_Manager[0]^1;
		On_Off_LED(Key_Manager[0],Key_Manager[2],Key_Manager[1]);
		*ASCII_Code=_Key_Table[ScanCode].NormalKey;
		_Flag=true;
		return _Flag;
	}
	else if(ScanCode==0x46)//Scroll Lock
	{
		Key_Manager[1]=Key_Manager[1]^1;
		On_Off_LED(Key_Manager[0],Key_Manager[2],Key_Manager[1]);
		*ASCII_Code=_Key_Table[ScanCode].NormalKey;
		_Flag=true;
		return _Flag;
	}
	else if(ScanCode==0x3A)// Caps Lock
	{
		Key_Manager[2]=Key_Manager[2]^1;
		On_Off_LED(Key_Manager[0],Key_Manager[2],Key_Manager[1]);
		*ASCII_Code=_Key_Table[ScanCode].NormalKey;
		_Flag=true;
		return _Flag;
	}
	
	if(CheckisNumPad(ScanCode))
	{
		if(Shift_Down^Key_Manager[0])
			*ASCII_Code=_Key_Table[ScanCode].Shift_Num_LockKey;
		else
			*ASCII_Code=_Key_Table[ScanCode].NormalKey;
	}
	else if(CheckisDual(ScanCode))
	{
		if(Shift_Down^Key_Manager[2])
			*ASCII_Code=_Key_Table[ScanCode].Shift_Num_LockKey;
		else 
			*ASCII_Code=_Key_Table[ScanCode].NormalKey;
	}
	else
	{
		*ASCII_Code=_Key_Table[ScanCode].NormalKey;
	}
		if(ScanCode&0x80)
			_Flag=false;
		else
			_Flag=true;
	return _Flag;
}

bool CheckisDual(uchar ScanCode)
{
	if(_Key_Table[ScanCode].NormalKey!=_Key_Table[ScanCode].Shift_Num_LockKey)
		return true;
	return false;
}

bool CheckisNumPad(uchar ScanCode)
{
	if(ScanCode>=71 && ScanCode<=83)
		return true;
	return false;
}

