#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

void skip_whitespace()
{
	int ch;
	while((ch = getc(stdin)) != EOF && isspace(ch))
		putc(ch, stdout);
	if(ch != EOF)
		ungetc(ch, stdin);
}

void unexpected(int ch)
{
	if(ch == EOF)
		fprintf(stderr, "Unexpected EOF\n");
	else
		fprintf(stderr, "Unexpected %c\n", ch);
	exit(1);
}

void expect(const char *s)
{
	while(*s)
	{
		int ch = getc(stdin);
		if(ch != *s)
			unexpected(ch);
		s++;
	}
}

void do_value();

void do_array()
{
	skip_whitespace();
	int ch = getc(stdin);
	if(ch == ']')
	{
		putc('}', stdout);
		return;
	}
	ungetc(ch, stdin);

	for(;;)
	{
		do_value();
		skip_whitespace();
		ch = getc(stdin);
		if(ch == ']')
			break;
		else if(ch != ',')
		{
			unexpected(ch);
			if(ch == EOF)
				break;
		}
		
		putc(',', stdout);
	}
	putc('}', stdout);
}

void do_object()
{
	skip_whitespace();
	int ch = getc(stdin);
	if(ch == '}')
	{
		putc('}', stdout);
		return;
	}
	ungetc(ch, stdin);

	for(;;)
	{
		skip_whitespace();
		putc('[', stdout);
		do_value();
		putc(']', stdout);
		putc(' ', stdout);
		putc('=', stdout);
		
		skip_whitespace();
		ch = getc(stdin);
		if(ch != ':')
			unexpected(ch);
		
		skip_whitespace();
		do_value();
		skip_whitespace();
		
		ch = getc(stdin);
		if(ch == '}')
			break;
		else if(ch != ',')
		{
			unexpected(ch);
			if(ch == EOF)
				break;
		}
		putc(',', stdout);
	}
	putc('}', stdout);
}

void do_string()
{
	int ch, k;
	putc('"', stdout);
	while((ch = getc(stdin)) != EOF && ch != '"')
	{
		putc(ch, stdout);
		if(ch == '\\')
		{
			ch = getc(stdin);
			if(ch != 'u')
				putc(ch, stdout);
			else {
				putc('x', stdout);
				if(getc(stdin) != '0') fprintf(stderr, "Only 8-bit characters supported\n");
				if(getc(stdin) != '0') fprintf(stderr, "Only 8-bit characters supported\n");
				putc(getc(stdin), stdout);
				putc(getc(stdin), stdout);
			}
		}
	}
	if(ch != '"')
		unexpected(ch);
	putc('"', stdout);
}

void do_value()
{
	skip_whitespace();
	int ch;
	switch(ch = getc(stdin))
	{
	case '[':
		putc('{', stdout);
		do_array();
		break;
	case '{':
		putc('{', stdout);
		do_object();
		break;
	case '"':
		do_string();
		break;
	case 'f': expect("alse");
		printf("false");
		break;
	case 't': expect("rue");
		printf("true");
		break;
	case 'n': expect("ull");
		printf("nil");
		break;
	default:
		unexpected(ch);
		break;
	}
}

int main()
{
	printf("return ");
	do_value();

	return 0;
}
