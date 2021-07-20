/*Code is inspired by :https://github.com/winobes/libcsv*/
// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2020-2021 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
 *******************************************************************************/
// DOM-IGNORE-END
#include "csv.h"
#include "definitions.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#ifdef SYS_OTA_APPDEBUG_ENABLED
#define DEBUG  
#endif

int add_char(char **string, int *c, char ch) {
    char *tmp = NULL;
    (*c)++;
    tmp = realloc(*string, (*c) + 1);
    if (tmp == NULL)
        return 1;
    *string = tmp;
    (*string)[(*c) - 1] = ch;
    (*string)[*c] = '\0';

    return 0;
}

CSV_FIELD *create_field() {
    CSV_FIELD *field = OSAL_Malloc(sizeof (CSV_FIELD));
    field->length = 0;
    field->text = NULL;
    set_field(field, "\0");
    return field;
}

void destroy_field(CSV_FIELD *field) {
    if (field->text != NULL) {
        OSAL_Free(field->text);
        field->text = NULL;
    }
    OSAL_Free(field);
    field = NULL;
}

int set_field(CSV_FIELD *field, char *text) {

    char *tmp;

    field->length = strlen(text) + 1;
    tmp = realloc(field->text, field->length);
    if (tmp == NULL)
        return 1;
    field->text = tmp;
    strcpy(field->text, text);

    return 0;
}
int read_byte_cnt = 0;

int read_next_field(SYS_FS_HANDLE fp,
        char field_delim, char text_delim,
        CSV_FIELD *field) {

    char ch = 'a';

    bool done = false;
    bool in_text = false;
    bool esc = false;

    int c = 0;
    char *tmp = OSAL_Malloc(1);
    tmp[0] = '\0';

    while (!done) {
        if (SYS_FS_FileEOF(fp)) {
#ifdef DEBUG
            printf("\n\nEOF reached\n\n");
#endif
            c = 0;
            done = true;
        }
        else {
            SYS_FS_FileRead(fp, &ch, 1);
#ifdef DEBUG
            printf("%c", ch);
#endif
            read_byte_cnt++;
            if (!in_text) {
                if (ch == text_delim) {
                    in_text = true;
                    c = 0;
                } else if (ch == field_delim) {
                    done = true;
                } else if (ch == '\n') {
                    done = true;
                } else {
                    add_char(&tmp, &c, ch);
                }
            } else { /* in_text == true */
                if (esc) {
                    if (ch == text_delim) {
                        add_char(&tmp, &c, ch);
                        esc = false;
                    } else {
                        esc = false;
                        done = true;
                    }
                } else { /* !esc */
                    if (ch == text_delim) {
                        esc = true;
                    } else if (ch == field_delim) {
                        add_char(&tmp, &c, ch);
                    } else {
                        add_char(&tmp, &c, ch);
                    }
                }
            }
        }
    }
    if (field != NULL) {
        set_field(field, tmp);
    }

    if (tmp != NULL)
        OSAL_Free(tmp);
    tmp = NULL;
#ifdef DEBUG
    printf("%c", ch);
#endif
    //SYS_FS_FileSeek( fp, 0, SYS_FS_SEEK_SET );
    /* Moving the fp to the beginning of the next field and peeking 
     * to see if it is a new line or if there is in fact no next field.
     */
    //fpos_t pos;
    int retval;
    done = false;
    while (!done) {

        //SYS_FS_FileRead(fp,&ch,1);
        //SYS_FS_FileSeek( fp, read_byte_cnt, SYS_FS_SEEK_SET );
        //read_byte_cnt++;
        if (ch == field_delim) {
            retval = 0;
            done = true;
        } else if (ch == '\n') {
            /* Checking to see if this \n is the one that UNIX
             * sometimes includes before the EOF */
            //fgetpos(fp, &pos);

            //SYS_FS_FileSeek( fp, read_byte_cnt, SYS_FS_SEEK_SET );

            //SYS_FS_FileRead(fp,&ch,1);;
            if (SYS_FS_FileEOF(fp))
                retval = 2;
            else
                retval = 1;
            //fsetpos(fp, &pos);
            done = true;
        } else if (SYS_FS_FileEOF(fp)) {
            retval = 2;
            done = true;
        } else {
            //SYS_FS_FileSeek( fp, read_byte_cnt, SYS_FS_SEEK_SET );
            SYS_FS_FileRead(fp, &ch, 1);
        }
#ifdef DEBUG
        printf("%c", ch);
#endif
    }
#ifdef DEBUG
    printf("%d", retval);
#endif
    //while(1);
    return retval;
}

int append_field(CSV_BUFFER *buffer, size_t row) {

    CSV_FIELD **temp_field;

    if (buffer->rows < row + 1)
        return 1;

    /* Set col equal to the index of the new field */
    int col = buffer->width[row];

    temp_field = realloc(buffer->field[row],
            (col + 1) * sizeof (CSV_FIELD*));
    if (temp_field == NULL) {
        return 2;
    } else {
        buffer->field[row] = temp_field;
        buffer->field[row][col] = create_field();
        buffer->width[row]++;
    }

    return 0;
}

int append_row(CSV_BUFFER *buffer) {
    size_t *temp_width;
    CSV_FIELD ***temp_field;

    size_t row = buffer->rows;

    temp_width = realloc(buffer->width, (buffer->rows + 1) *
            sizeof (size_t));
    if (temp_width != NULL) {
        buffer->width = temp_width;
        buffer->width[row] = 0;
    } else {
        return 1;
    }

    temp_field = realloc(buffer->field, (buffer->rows + 1) *
            sizeof (CSV_FIELD**));
    if (temp_field != NULL) {
        buffer->field = temp_field;
        buffer->field[row] = NULL;
    } else {
        OSAL_Free(temp_width);
        return 2;
    }

    buffer->rows++;
    append_field(buffer, row);
    return 0;
}

int remove_last_field(CSV_BUFFER *buffer, size_t row) {

    size_t entry = buffer->width[row] - 1;

    CSV_FIELD **temp_row;

    /* If there are no entries in the row there is nothing to
     * remove, but return success because this is expected */
    if (row > buffer->rows - 1)
        return 0;
        /* If t he row exists, but has no fields, something went
         * wrong; every row in the scope should have at least one
         * field. 
         */
    else if (buffer->width[row] == 0)
        return 1;
        /* If there is only one entry left, just clear it. If the 
         * caller wants to remove the row, she will call
         * csv_remove_row.
         */
    else if (buffer->width[row] == 1) {
        csv_clear_field(buffer, row, 0);
        return 0;
    }        /* Otherwise destroy the final field and decrement the width */
    else {
        destroy_field(buffer->field[row][entry]);
        temp_row = realloc(buffer->field[row], entry
                * sizeof (CSV_FIELD*));
        if (temp_row != NULL)
            buffer->field[row] = temp_row;
        else
            return 3;
        buffer->width[row]--;
    }

    return 0;
}

int remove_last_row(CSV_BUFFER *buffer) {

    int row = buffer->rows - 1;
    int entry = buffer->width[row] - 1;

    CSV_FIELD ***temp_field = NULL;
    size_t *temp_width = NULL;

    while (entry >= 0) {
        remove_last_field(buffer, row);
        entry--;
    }

    temp_field = realloc(buffer->field, (buffer->rows - 1) *
            sizeof (CSV_FIELD**));
    temp_width = realloc(buffer->width, (buffer->rows - 1) *
            sizeof (size_t));
    if (temp_width == NULL || temp_field == NULL)
        return 1;

    buffer->field = temp_field;
    buffer->width = temp_width;
    buffer->rows--;

    return 0;
}

CSV_BUFFER *csv_create_buffer() {

    CSV_BUFFER *buffer = OSAL_Malloc(sizeof (CSV_BUFFER));

    if (buffer != NULL) {
        buffer->field = NULL;
        buffer->rows = 0;
        buffer->width = NULL;
        buffer->field_delim = ',';
        buffer->text_delim = '"';
    }

    return buffer;
}

void csv_destroy_buffer(CSV_BUFFER *buffer) {

    int i, j;

    for (i = 0; i < buffer->rows; i++) {
        for (j = 0; j < buffer->width[i]; j++) {
            destroy_field(buffer->field[i][j]);
        }
        OSAL_Free(buffer->field[i]);
        buffer->field[i] = NULL;
    }

    if (buffer->field != NULL)
        OSAL_Free(buffer->field);

    if (buffer->width != NULL)
        OSAL_Free(buffer->width);

    OSAL_Free(buffer);
}
SYS_FS_HANDLE fileHandle2;

int csv_load(CSV_BUFFER *buffer, char *file_name) {

    fileHandle2 = SYS_FS_FileOpen(file_name, (SYS_FS_FILE_OPEN_READ));
    if (fileHandle2 == SYS_FS_HANDLE_INVALID) {
        /* File open unsuccessful */
        #ifdef DEBUG
        printf("File Open Failed");
        #endif
        return 1;
    } else {
        /* File open was successful. Write to the file. */
        #ifdef DEBUG
        printf("File Opened");
        #endif
    }

    read_byte_cnt = 0;
    int next = 1;
    bool end = false;
    bool first = true;
    int i = -1, j = -1;

    while (!end) {

        if (!first) {
            next = read_next_field(fileHandle2,
                    buffer->field_delim, buffer->text_delim,
                    buffer->field[i][j - 1]);
        }

        if (next == 2) {
            end = true;

        }
        if (next == 1) {

            if (append_row(buffer) != 0)
                return 2;
            j = 1;
            i++;
        }

        if (next == 0) {

            if (append_field(buffer, i) != 0)
                return 2;
            j++;

        }
        if (first) first = false;
    }

    SYS_FS_FileClose(fileHandle2);

    return 0;
}

int csv_save(char *file_name, CSV_BUFFER *buffer) {

    int i, j, k;
    char *chloc;
    #ifdef DEBUG
    char read_buf[200];
    char read_buf1[200];
    #endif
    //FILE *fp = fopen(file_name, "w");
    fileHandle2 = SYS_FS_FileOpen(file_name, (SYS_FS_FILE_OPEN_WRITE));
    if (fileHandle2 == SYS_FS_HANDLE_INVALID) {
        /* File open unsuccessful */
        #ifdef DEBUG
        printf("File Open Failed\n");
        #endif
        return 1;
    } else {
        /* File open was successful. Write to the file. */
        #ifdef DEBUG
        printf("File Opened\n");
        
        SYS_FS_FileSeek(fileHandle2, 0, SYS_FS_SEEK_SET);
        SYS_FS_FileRead(fileHandle2, read_buf, 100);
        
        printf("reading file content before modification %s\n", read_buf);
        #endif
        //file_opened = true;

    }
    /*if (fp == NULL)
            return 1;*/
    uint8_t read_byte_cnt = 0;
    char text_delim = buffer->text_delim;
    char field_delim = buffer->field_delim;
    SYS_FS_FileSeek(fileHandle2, read_byte_cnt, SYS_FS_SEEK_SET);
    for (i = 0; i < buffer->rows; i++) {
        for (j = 0; j < buffer->width[i]; j++) {
            //printf("buffer->field[i][j]->text : %s\n",buffer->field[i][j]->text);
            chloc = strchr(buffer->field[i][j]->text, text_delim);
            if (chloc == NULL) {
                #ifdef DEBUG
                printf("Searching field_delim : %s\n", buffer->field[i][j]->text);
                #endif
                chloc = strchr(buffer->field[i][j]->text, field_delim);
                //if (chloc != NULL)
                    #ifdef DEBUG
                    printf("chloc is set\n");
                    #endif

            }
            if (chloc == NULL) {
                #ifdef DEBUG
                printf("Searching new line : %s\n", buffer->field[i][j]->text);
                #endif
                chloc = strchr(buffer->field[i][j]->text, '\n');
                #ifdef DEBUG
                if (chloc != NULL)
                    printf("chloc is set\n");
                #endif
            }
            /* if any of the above characters are found, chloc will be set
             * and we must use text deliminators.
             */
            
            if (chloc != NULL) {
                #ifdef DEBUG
                printf("chloc != NULL \n");
                #endif    

                SYS_FS_FileWrite(fileHandle2, &text_delim, 1);
                read_byte_cnt++;
                SYS_FS_FileSeek(fileHandle2, read_byte_cnt, SYS_FS_SEEK_SET);
                #ifdef DEBUG
                printf("%c", text_delim);
                #endif
                //fputc(text_delim, fp);
                for (k = 0; k < buffer->field[i][j]->length - 1; k++) {
                    /* if there are any text delims in the string,
                     * we must escape them.
                     */
                    if (buffer->field[i][j]->text[k] == text_delim) {
                        SYS_FS_FileWrite(fileHandle2, &text_delim, 1);
                        read_byte_cnt++;
                        SYS_FS_FileSeek(fileHandle2, read_byte_cnt, SYS_FS_SEEK_SET);
                        #ifdef DEBUG
                        printf("%c", text_delim);
                        #endif
                    }
                    //fputc(text_delim, fp);
                    SYS_FS_FileWrite(fileHandle2, &buffer->field[i][j]->text[k], 1);
                    read_byte_cnt++;
                    SYS_FS_FileSeek(fileHandle2, read_byte_cnt, SYS_FS_SEEK_SET);
                    #ifdef DEBUG
                    printf("%c", buffer->field[i][j]->text[k]);
                    #endif
                    //fputc(buffer->field[i][j]->text[k], fp);
                }

                SYS_FS_FileWrite(fileHandle2, &text_delim, 1);
                read_byte_cnt++;
                SYS_FS_FileSeek(fileHandle2, read_byte_cnt, SYS_FS_SEEK_SET);
                #ifdef DEBUG
                printf("%c\n", text_delim);
                #endif
                //fputc(text_delim, fp);
                chloc = NULL;
            } else {
                #ifdef DEBUG
                printf("writing string : %s", buffer->field[i][j]->text);
                #endif
                SYS_FS_FileWrite(fileHandle2, buffer->field[i][j]->text, strlen(buffer->field[i][j]->text));
                read_byte_cnt += strlen(buffer->field[i][j]->text);
                SYS_FS_FileSeek(fileHandle2, read_byte_cnt, SYS_FS_SEEK_SET);
                //fputs(buffer->field[i][j]->text, fp);
            }
            if (j < buffer->width[i] - 1) {
                SYS_FS_FileWrite(fileHandle2, &field_delim, 1);
                read_byte_cnt++;
                SYS_FS_FileSeek(fileHandle2, read_byte_cnt, SYS_FS_SEEK_SET);
                #ifdef DEBUG
                printf("writing field delimeter %c", field_delim);
                #endif
            }                //fputc(field_delim, fp);
            else if (i < buffer->rows - 1) {
                char ch = '\n';
                SYS_FS_FileCharacterPut(fileHandle2, ch);
                read_byte_cnt++;
                SYS_FS_FileSeek(fileHandle2, read_byte_cnt, SYS_FS_SEEK_SET);
                #ifdef DEBUG
                printf("writing ne line %c", ch);
                #endif
            }
            //fputc('\n', fp);
        }
    }
    SYS_FS_FileClose(fileHandle2);
    #ifdef DEBUG
    fileHandle2 = SYS_FS_FileOpen(file_name, (SYS_FS_FILE_OPEN_READ));
    SYS_FS_FileSeek(fileHandle2, 0, SYS_FS_SEEK_SET);
    SYS_FS_FileRead(fileHandle2, read_buf1, 100);
    
    printf("reading file content affter modification %s\n", read_buf1);
    
    SYS_FS_FileClose(fileHandle2);
    #endif
    return 0;
}

int csv_get_field(char *dest, size_t dest_len,
        CSV_BUFFER *src, size_t row, size_t entry) {
    int i;
    if (dest_len == 0)
        return 3;
    if (row >= src->rows /*row does not exist*/
            || row < 0
            || entry >= src->width[row] /*entry does not exist*/
            || entry < 0) {
        for (i = 0; i < dest_len; i++)
            dest[0] = '\0';
        /* If the requested entry does not exist or is
         * invalid, we clear the string provided consistent
         * with the case of an empty entry.
         */
        return 2;
    } else {

        /* If destination is not large enough to hold the whole entry,
         * strncpy will truncate it for us. 
         */
        strncpy(dest, src->field[row][entry]->text, dest_len);
        dest[dest_len] = '\0';
    }

    if (src->field[row][entry]->length > dest_len + 1)
        return 1;
    if (src->field[row][entry]->length == 0)
        return 2;
    else
        return 0;
}

int csv_copy_field(CSV_BUFFER *dest, int dest_row, int dest_entry,
        CSV_BUFFER *source, int source_row, int source_entry) {
    return set_field(dest->field[dest_row][dest_entry],
            source->field[source_row][source_entry]->text);
}

int csv_clear_field(CSV_BUFFER *buffer, size_t row, size_t entry) {
    /* Field is already clear (out of range) */
    if (buffer->rows < row + 1 || buffer->width[row] < entry + 1)
        return 0;

    /* Destroy the field if it is last in the row (and now field 0) */
    if (entry == buffer->width[row] - 1 && entry != 0)
        remove_last_field(buffer, row);

    else
        set_field(buffer->field[row][entry], "\0");

    return 0;
}

int csv_clear_row(CSV_BUFFER *buffer, size_t row) {

    CSV_FIELD **temp_row;

    /* If the requested field is the last one, simply remove it. */
    if (row == buffer->rows - 1) {
        if (remove_last_row(buffer) != 0)
            return 1;
        else
            return 0;
    }

    /* Destroy every field but the last one */
    int i;
    for (i = buffer->width[row] - 1; i > 0; i--) {
        destroy_field(buffer->field[row][i]);
    }
    /* Clear the last field */
    set_field(buffer->field[row][0], "\0");

    temp_row = realloc(buffer->field[row], sizeof (CSV_FIELD*));
    /* If it didn't shrink, recreate the destroyed fields */
    if (temp_row == NULL) {
        for (i = 1; i < buffer->width[row]; i++) {
            append_field(buffer, row);
            set_field(buffer->field[row][i], "\0");
        }
        return 1;
    } else {
        buffer->field[row] = temp_row;
    }

    buffer->width[row] = 1;

    return 0;
}

int csv_copy_row(CSV_BUFFER *dest, int dest_row,
        CSV_BUFFER *source, int source_row) {

    if (source_row > source->rows - 1) {
        csv_clear_row(dest, dest_row);
        return 0;
    }

    while (dest->rows < (dest_row + 1))
        if (append_row(dest) != 0)
            return 1;

    while (dest->width[dest_row] > source->width[source_row])
        if (remove_last_field(dest, dest_row) != 0)
            return 1;

    while (dest->width[dest_row] < source->width[source_row])
        if (append_field(dest, dest_row) != 0)
            return 1;

    int i;
    for (i = 0; i < dest->width[dest_row]; i++)
        csv_copy_field(dest, dest_row, i, source, source_row, i);

    return 0;
}

int csv_remove_row(CSV_BUFFER *buffer, size_t row) {

    if (row > buffer->rows - 1)
        return -1;

    int i;
    for (i = row; i < buffer->rows - 1; i++)
        csv_copy_row(buffer, i, buffer, i + 1);
    remove_last_row(buffer);

    return 0;

}

int csv_remove_field(CSV_BUFFER *buffer, size_t row, size_t entry) {
    if (row > buffer->rows - 1 || entry > buffer->width[row] - 1)
        return 0;

    int i;
    for (i = entry; i < buffer->width[row] - 1; i++)
        csv_copy_field(buffer, row, i, buffer, row, i + 1);
    remove_last_field(buffer, row);

    return 0;
}

void csv_set_text_delim(CSV_BUFFER *buffer, char new_delim) {
    buffer->text_delim = new_delim;
}

void csv_set_field_delim(CSV_BUFFER *buffer, char new_delim) {
    buffer->field_delim = new_delim;
}

int csv_get_height(CSV_BUFFER *buffer) {
    return buffer->rows;
}

int csv_get_width(CSV_BUFFER *buffer, size_t row) {
    if (row > buffer->rows - 1)
        return 0;
    else
        return buffer->width[row];
}

int csv_get_field_length(CSV_BUFFER *buffer, size_t row, size_t entry) {

    if (row > buffer->rows - 1)
        return 0;
    else if (entry > buffer->width[row] - 1)
        return 0;
    else
        return buffer->field[row][entry]->length - 1;
}

int csv_set_field(CSV_BUFFER *buffer, size_t row, size_t entry,
        char *field) {

    while (row >= buffer->rows) {
        append_row(buffer);
    }
    while (entry >= buffer->width[row])
        append_field(buffer, row);

    if (set_field(buffer->field[row][entry], field) == 0)
        return 0;
    else
        return 1;
}

int csv_insert_field(CSV_BUFFER *buffer, size_t row, size_t entry,
        char *field) {
    /* If the field does not exist, simply set it */
    if (row > buffer->rows - 1
            || entry > buffer->width[row] - 1)
        csv_set_field(buffer, row, entry, field);

        /* Otherwise move everything over, then set it */
    else {
        append_field(buffer, row);
        int i = 0;
        for (i = buffer->width[row] - 1; i > entry; i--)
            csv_copy_field(buffer, row, i,
                buffer, row, i - 1);
        csv_set_field(buffer, row, entry, field);
    }

    return 0;
}