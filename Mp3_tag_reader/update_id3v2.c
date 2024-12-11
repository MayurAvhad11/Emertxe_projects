#include "main.h"

// Function to read a sync-safe integer (used for tag size and frame sizes)
int read_syncsafe_int(const unsigned char *data)
{
    return (data[0] & 0x7F) << 21 | (data[1] & 0x7F) << 14 |
           (data[2] & 0x7F) << 7 | (data[3] & 0x7F);
}

// Function to write a sync-safe integer
void write_syncsafe_int(unsigned char *data, int value)
{
    data[0] = (value >> 21) & 0x7F;
    data[1] = (value >> 14) & 0x7F;
    data[2] = (value >> 7) & 0x7F;
    data[3] = value & 0x7F;
}

// Function to parse frames from tag data
int parse_frames(const unsigned char *tag_data, int tag_size, Frame **frames)
{
    int offset = 0, frame_count = 0;
    *frames = NULL;

    while (offset < tag_size)
    {
        const unsigned char *frame_header = tag_data + offset;

        // Stop parsing at padding (frame ID is 0)
        if (frame_header[0] == 0)
            break;

        // Allocate memory for a new frame
        Frame frame;
        strncpy(frame.id, (char *)frame_header, 4);
        frame.id[4] = '\0';
        frame.size = (frame_header[4] << 24) | (frame_header[5] << 16) |
                     (frame_header[6] << 8) | (frame_header[7]);
        frame.content = malloc(frame.size);
        if (!frame.content)
        {
            perror("Memory allocation failed");
            return -1;
        }
        memcpy(frame.content, tag_data + offset + FRAME_HEADER_SIZE, frame.size);

        // Append frame to the list
        *frames = realloc(*frames, sizeof(Frame) * (frame_count + 1));
        (*frames)[frame_count++] = frame;

        // Move to the next frame
        offset += FRAME_HEADER_SIZE + frame.size;
    }

    return frame_count;
}

// Function to reconstruct tag data
int reconstruct_tag(unsigned char **tag_data, Frame *frames, int frame_count)
{
    int new_tag_size = 0;

    // Calculate total size
    for (int i = 0; i < frame_count; i++)
    {
        new_tag_size += FRAME_HEADER_SIZE + frames[i].size;
    }

    // Add padding (optional, e.g., 1024 bytes)
    int padding = 1024;
    new_tag_size += padding;

    // Allocate memory for the new tag
    *tag_data = calloc(new_tag_size, 1);
    if (!*tag_data)
    {
        perror("Memory allocation failed");
        return -1;
    }

    // Write frames
    int offset = 0;
    for (int i = 0; i < frame_count; i++)
    {
        memcpy(*tag_data + offset, frames[i].id, 4);
        (*tag_data)[offset + 4] = (frames[i].size >> 24) & 0xFF;
        (*tag_data)[offset + 5] = (frames[i].size >> 16) & 0xFF;
        (*tag_data)[offset + 6] = (frames[i].size >> 8) & 0xFF;
        (*tag_data)[offset + 7] = frames[i].size & 0xFF;
        memcpy(*tag_data + offset + FRAME_HEADER_SIZE, frames[i].content, frames[i].size);
        offset += FRAME_HEADER_SIZE + frames[i].size;
    }

    return new_tag_size;
}

// Function to update a specific ID3v2 tag
int update_id3v2_tag(const char *filename, const char *tag, const char *new_value)
{
    FILE *file = fopen(filename, "rb+");
    if (!file)
    {
        perror("Error opening file");
        return FAILURE;
    }

    // Read the ID3v2 header
    unsigned char header[HEADER_SIZE];
    fread(header, 1, HEADER_SIZE, file);
    if (strncmp((char *)header, "ID3", 3) != 0)
    {
        printf("ID3v2 tag not found.\n");
        fclose(file);
        return FAILURE;
    }

    // Read the tag size
    int tag_size = read_syncsafe_int(header + 6);
    unsigned char *tag_data = malloc(tag_size);
    if (!tag_data)
    {
        perror("Memory allocation failed");
        fclose(file);
        return FAILURE;
    }
    fread(tag_data, 1, tag_size, file);

    // Parse frames
    Frame *frames = NULL;
    int frame_count = parse_frames(tag_data, tag_size, &frames);
    if (frame_count < 0)
    {
        free(tag_data);
        fclose(file);
        return FAILURE;
    }

    // Update the target frame
    for (int i = 0; i < frame_count; i++)
    {
        if (strcmp(frames[i].id, tag) == 0)
        {
            printf("Found tag %s, updating...\n", tag);
            free(frames[i].content);
            frames[i].size = strlen(new_value) + 1;
            frames[i].content = malloc(frames[i].size);
            frames[i].content[0] = 0x00; // Encoding (UTF-8)
            strcpy(frames[i].content + 1, new_value);
            break;
        }
    }

    // Reconstruct the tag
    unsigned char *new_tag_data = NULL;
    int new_tag_size = reconstruct_tag(&new_tag_data, frames, frame_count);
    if (new_tag_size < 0)
    {
        free(tag_data);
        fclose(file);
        return FAILURE;
    }

    // Update the header with the new tag size
    write_syncsafe_int(header + 6, new_tag_size);

    // Write everything back to the file
    fseek(file, 0, SEEK_SET);
    fwrite(header, 1, HEADER_SIZE, file);
    fwrite(new_tag_data, 1, new_tag_size, file);

    // Cleanup
    free(tag_data);
    free(new_tag_data);
    for (int i = 0; i < frame_count; i++)
    {
        free(frames[i].content);
    }
    free(frames);
    fclose(file);

    // printf("Tag updated successfully.\n");
    return SUCCESS;
}
