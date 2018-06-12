#ifndef _SCSI_SCSI_CMND_H
#define _SCSI_SCSI_CMND_H

#include <linux/dma-mapping.h>
#include <linux/list.h>
#include <linux/types.h>
#include <linux/completion.h>

struct scsi_device;

struct scsi_cmnd
{
	struct scsi_device *device;

#define MAX_COMMAND_SIZE	16
	unsigned char cmnd[MAX_COMMAND_SIZE];

	unsigned char cmd_len;
	unsigned char old_cmd_len;

	unsigned long serial_number;
	unsigned underflow;

	enum dma_data_direction sc_data_direction;
	enum dma_data_direction sc_old_data_direction;
	unsigned short use_sg;
	int resid;

	void *request_buffer;		/* Actual requested buffer */
	unsigned request_bufflen;	/* Actual request size */

	unsigned bufflen;	/* Size of data buffer */
	void *buffer;		/* Data buffer */

	/* obtained by REQUEST SENSE
	 * when CHECK CONDITION is
	 * received on original command
	 * (auto-sense)
	 */
#define SCSI_SENSE_BUFFERSIZE 	96
	unsigned char sense_buffer[SCSI_SENSE_BUFFERSIZE];
	/* obtained by REQUEST SENSE
	 * when CHECK CONDITION is
	 * received on original command
	 * (auto-sense)
	 */

	int result;		/* Status code from lower level driver */
};

/*
 * These are the values that scsi_cmd->state can take.
 */
#define SCSI_STATE_TIMEOUT         0x1000
#define SCSI_STATE_FINISHED        0x1001
#define SCSI_STATE_FAILED          0x1002
#define SCSI_STATE_QUEUED          0x1003
#define SCSI_STATE_UNUSED          0x1006
#define SCSI_STATE_DISCONNECTING   0x1008
#define SCSI_STATE_INITIALIZING    0x1009
#define SCSI_STATE_BHQUEUE         0x100a
#define SCSI_STATE_MLQUEUE         0x100b


extern struct scsi_cmnd *scsi_get_command(struct scsi_device *, int);
extern void scsi_put_command(struct scsi_cmnd *);
extern void scsi_io_completion(struct scsi_cmnd *, unsigned int, unsigned int);
extern void scsi_finish_command(struct scsi_cmnd *cmd);

#endif /* _SCSI_SCSI_CMND_H */
