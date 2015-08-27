#pragma once

#include <protobuf-c/protobuf-c.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct zpbc_t	zpbc_t;

/* ---------------------------------------*/
/**
 * @Synopsis zpbc_new 
 *		create a zprotobuf instance. note,all those methord is NOT multi-thread safe.
 *
 * @Returns   
 */
/* ---------------------------------------*/
zpbc_t* zpbc_new();

/* ---------------------------------------*/
/**
 * @Synopsis zpbc_destroy 
 *		destroy zprotobuf instance
 *
 * @Param p_self
 */
/* ---------------------------------------*/
void zpbc_destroy(zpbc_t** p_self);

/* ---------------------------------------*/
/**
 * @Synopsis zpbc_register
 *
 * @Param self
 * @Param descriptor
 *
 * @Returns   
 *		-1		bad param
 *		0		register success
 */
/* ---------------------------------------*/
int zpbc_register(zpbc_t* self,const ProtobufCMessageDescriptor* descriptor);

/* ---------------------------------------*/
/**
 * @Synopsis zpbc_send 
 *		if only sending data,no need to create zpbc_t instance.
 *
 * @Param m
 * @Param dest
 * @Param more
 *
 * @Returns   
 */
/* ---------------------------------------*/
int zpbc_send(const ProtobufCMessage* m,void* dest,int more);


/* ---------------------------------------*/
/**
 * @Synopsis zpbc_recv 
 *		recv but not deliver.
 *
 * @Param self
 * @Param source
 *
 * @Returns   ProtobufCMessage* if success,should call protobuf_c_free_unpacked to free.
 */
/* ---------------------------------------*/
ProtobufCMessage* zpbc_recv(zpbc_t* self,void* source);




#ifdef __cplusplus
}
#endif


