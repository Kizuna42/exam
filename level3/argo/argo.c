#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

typedef struct	json {
	enum {
		MAP,
		INTEGER,
		STRING
	} type;
	union {
		struct {
			struct pair	*data;
			size_t		size;
		} map;
		int	integer;
		char	*string;
	};
}	json;

typedef struct	pair {
	char	*key;
	json	value;
}	pair;

void	free_json(json j);
int	argo(json *dst, FILE *stream);

int	peek(FILE *stream)
{
	int	c = getc(stream);
	ungetc(c, stream);
	return c;
}

void	unexpected(FILE *stream)
{
	if (peek(stream) != EOF)
		printf("Unexpected token '%c'\n", peek(stream));
	else
		printf("Unexpected end of input\n");
}

int	accept(FILE *stream, char c)
{
	if (peek(stream) == c)
	{
		(void)getc(stream);
		return 1;
	}
	return 0;
}

int	expect(FILE *stream, char c)
{
	if (accept(stream, c))
		return 1;
	unexpected(stream);
	return 0;
}

int parse_string(FILE *stream, char **str)
{
    if (!expect(stream, '"')) return 0;
    
    char *buf = malloc(1000);
    int len = 0;
    
    while (peek(stream) != '"' && peek(stream) != EOF)
    {
        char c = getc(stream);
        if (c == '\\')
        {
            c = getc(stream);
            if (c == EOF) { free(buf); unexpected(stream); return 0; }
        }
        buf[len++] = c;
    }
    
    if (!expect(stream, '"')) { free(buf); return 0; }
    buf[len] = '\0';
    *str = buf;
    return 1;
}

int parse_number(FILE *stream, int *num)
{
    if (!isdigit(peek(stream))) return 0;
    return fscanf(stream, "%d", num) == 1;
}

int parse_value(FILE *stream, json *dst);

int parse_map(FILE *stream, json *dst)
{
    if (!expect(stream, '{')) return 0;
    
    dst->type = MAP;
    dst->map.data = malloc(100 * sizeof(pair));
    dst->map.size = 0;
    
    if (accept(stream, '}')) return 1;
    
    do {
        char *key;
        if (!parse_string(stream, &key)) return 0;
        if (!expect(stream, ':')) return 0;
        
        dst->map.data[dst->map.size].key = key;
        if (!parse_value(stream, &dst->map.data[dst->map.size].value)) return 0;
        dst->map.size++;
    } while (accept(stream, ','));
    
    return expect(stream, '}');
}

int parse_value(FILE *stream, json *dst)
{
    if (peek(stream) == '"')
    {
        dst->type = STRING;
        return parse_string(stream, &dst->string);
    }
    else if (isdigit(peek(stream)))
    {
        dst->type = INTEGER;
        return parse_number(stream, &dst->integer);
    }
    else if (peek(stream) == '{')
    {
        return parse_map(stream, dst);
    }
    
    unexpected(stream);
    return 0;
}

int argo(json *dst, FILE *stream)
{
    return parse_value(stream, dst);
}

void	free_json(json j)
{
	switch (j.type)
	{
		case MAP:
			for (size_t i = 0; i < j.map.size; i++)
			{
				free(j.map.data[i].key);
				free_json(j.map.data[i].value);
			}
			free(j.map.data);
			break ;
		case STRING:
			free(j.string);
			break ;
		default:
			break ;
	}
}

void	serialize(json j)
{
	switch (j.type)
	{
		case INTEGER:
			printf("%d", j.integer);
			break ;
		case STRING:
			putchar('"');
			for (int i = 0; j.string[i]; i++)
			{
				if (j.string[i] == '\\' || j.string[i] == '"')
					putchar('\\');
				putchar(j.string[i]);
			}
			putchar('"');
			break ;
		case MAP:
			putchar('{');
			for (size_t i = 0; i < j.map.size; i++)
			{
				if (i != 0)
					putchar(',');
				serialize((json){.type = STRING, .string = j.map.data[i].key});
				putchar(':');
				serialize(j.map.data[i].value);
			}
			putchar('}');
			break ;
	}
}

int	main(int argc, char **argv)
{
	if (argc != 2)
		return 1;
	char *filename = argv[1];
	FILE *stream = fopen(filename, "r");
	json	file;
	if (argo (&file, stream) != 1)
	{
		free_json(file);
		return 1;
	}
	serialize(file);
	printf("\n");
} 