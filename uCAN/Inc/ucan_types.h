/**
  ******************************************************************************
  * @file    ucan_types.h
  * @author  Hamza Enes Balahoroğlu
  * @brief   Type definitions and data structures for the uCAN communication module.
  *          This file contains enums, structs, and typedefs used throughout the
  *          uCAN library to manage CAN bus communication, node roles, packet
  *          configuration, and status handling.
  *
  * @note    Designed to work with STM32 HAL CAN driver and compatible with
  *          embedded CAN applications requiring flexible data binding and
  *          node management.
  *
  *
  *                          _____          _   _
  *                         / ____|   /\   | \ | |
  *                   _   _| |       /  \  |  \| |
  *                  | | | | |      / /\ \ | . ` |
  *                  | |_| | |____ / ____ \| |\  |
  *                   \____|\_____/_/    \_\_| \_|
  *
  ******************************************************************************
  */

#ifndef UCAN_TYPES
#define UCAN_TYPES

#include "stm32f4xx_hal.h"

/**
  * @brief  Data type definition for CAN payload items.
  * @note   Used to indicate the size of the data associated with each CAN signal.
  */
typedef enum {
    UCAN_U8,   								/*!< 8-bit unsigned data (uint8_t) */
    UCAN_U16,  								/*!< 16-bit unsigned data (uint16_t) */
    UCAN_U32   								/*!< 32-bit unsigned data (uint32_t) */
} UCAN_DataType;

/**
  * @brief  Defines the role of a node on the CAN bus.
  * @note   Determines how the node behaves in the communication protocol.
  */
typedef enum {
    UCAN_ROLE_MASTER, 						/*!< Node initiates communication and manages handshakes */
    UCAN_ROLE_CLIENT,						/*!< Node responds to master's messages and sends periodic data */
    UCAN_ROLE_NONE							/*!< Role not assigned or inactive node */
} UCAN_NodeRole;


/**
  * @brief  Status codes returned by uCAN functions.
  * @note   Indicates the result of CAN operations and error conditions.
  */
typedef enum {
    UCAN_NOT_INITIALIZED  		= 0x00U,	/*!< uCAN module is not initialized */
    UCAN_OK               		= 0x01U,	/*!< Operation successful, data sent */
    UCAN_ERROR            		= 0x02U,	/*!< General failure during data transmission */
    UCAN_MISSING_VAL      		= 0x03U,	/*!< Data mismatch or invalid length specified */
    UCAN_NO_CONNECTION    		= 0x04U,	/*!< No connection on the CAN bus */
    UCAN_NO_CHANGED_VAL   		= 0x05U,	/*!< No change in value, transmission skipped */
    UCAN_TIMEOUT          		= 0x06U,	/*!< Operation timed out */
    UCAN_INVALID_PARAM    		= 0x07U,	/*!< Invalid parameter passed to function */
    UCAN_BUSY             		= 0x08U,  	/*!< CAN bus is busy, try again later */
    UCAN_ERROR_DUPLICATE_ID		= 0x09U,	/*!< Duplicate ID detected in client or packet list */
    UCAN_ERROR_FILTER_CONFIG	= 0x0AU, 	/*!< Failed to configure CAN filter settings */
    UCAN_ERROR_CAN_START		= 0x0BU, 	/*!< Error occurred while starting the CAN peripheral */
    UCAN_ERROR_CAN_NOTIFICATION	= 0x0CU, 	/*!< Failed to activate CAN RX/TX/FIFO notifications */
    UCAN_ERROR_UNKNOWN_ID		= 0x0DU		/*!< Provided ID does not match any known packet configuration */
} UCAN_StatusTypeDef;


/**
  * @brief  Connection status definitions for uCAN communication.
  * @note   Indicates the state of a node’s connection on the CAN bus.
  */
typedef enum {
    UCAN_CONN_WAITING    	= 0x00U,		/*!< Waiting for response, e.g., after handshake request */
    UCAN_CONN_ACTIVE       	= 0x01U,		/*!< Connection is active and communication is ongoing */
    UCAN_CONN_LOST   		= 0x02U,		/*!< Connection was previously active but is now lost */
    UCAN_CONN_TIMEOUT   	= 0x03U			/*!< No response received within the expected timeframe */
} UCAN_ConnectionStatusTypeDef;

/**
  * @brief  Structure to represent a generic data item in the CAN payload.
  * @note   Only supports unsigned integer types (uint8_t, uint16_t, uint32_t).
  *         The pointer must reference a variable that matches the declared type.
  */
typedef struct {
    void* ptr;								/*!< Pointer to the data value (e.g., &some_u8_var) */
    UCAN_DataType type;						/*!< Type of the data (UCAN_U8, UCAN_U16, UCAN_U32) */
} UCAN_Data;

/**
  * @brief  User-defined configuration for binding application variables to CAN messages.
  * @note   This structure is passed to uCAN_Start() to register signal mappings.
  *         It links application-level variables to internal transmission logic.
  *         After uCAN_Start() completes, this config is no longer used.
  */
typedef struct {
    uint32_t id;                    		/*!< CAN identifier associated with the signal group */
    uint8_t item_count;             		/*!< Number of data items (max 8) */
    UCAN_Data items[8];             		/*!< Array of data pointers and their types from the application */
} UCAN_PacketConfig;

/**
  * @brief  Internal representation of a raw CAN packet.
  * @note   Used by the uCAN core to construct and transmit actual CAN frames.
  *         Each byte in the payload is mapped to an external variable via pointers.
  */
typedef struct {
    uint32_t id;             				/*!< CAN identifier to be used for transmission */
    uint8_t dlc;              				/*!< Data length code (number of payload bytes: 0 to 8) */
    uint8_t* bits[8];         				/*!< Pointers to individual bytes forming the payload */
} UCAN_Packet;

/**
  * @brief  Container structure for managing multiple CAN packets.
  * @note   Holds the number of active packets and a pointer to an array of UCAN_Packet.
  *         Used internally to organize batch transmissions or packet management.
  */
typedef struct {
    uint32_t count;          				/*!< Number of CAN packets stored in the holder */
    UCAN_Packet* packets;    				/*!< Pointer to an array of UCAN_Packet structures */
} UCAN_PacketHolder;

/**
  * @brief  Represents a single client node in the CAN network.
  * @note   Stores the unique ID, last response time, and current connection status of the client.
  */
typedef struct {
    uint32_t id;                		 	/*!< Unique identifier for a specific client node */
    uint32_t responseTick;      		 	/*!< Timestamp (in ms) of the last received response from the client */
    UCAN_ConnectionStatusTypeDef status;	/*!< Current connection status of the client node */
} UCAN_Client;

/**
  * @brief  Structure containing information about a CAN node and its network clients.
  * @note   Manages node role, identifiers, and connection statuses of connected clients.
  */
typedef struct {
    UCAN_NodeRole role;						/*!< Role of this node on the CAN bus (Master, Client, None) */
    uint32_t selfId;						/*!< CAN identifier assigned to this node */
    uint32_t masterId;						/*!< CAN identifier of the master node */
    uint32_t sentTick;						/*!< Timestamp (in ms) of the last message sent by this node */
    UCAN_Client* clients;					/*!< Pointer to array of known clients in the network */
    uint32_t clientCount;					/*!< Number of clients in the clientIdList array */
} UCAN_NodeInfo;



/**
  * @brief  Configuration structure for uCAN module transmit and receive packets.
  * @note   Holds pointers to user-defined arrays of transmit and receive packet configurations.
  */
typedef struct {
    UCAN_PacketConfig* txPacketList; 		/*!< Pointer to an array of transmit packet configurations */
    UCAN_PacketConfig* rxPacketList; 		/*!< Pointer to an array of receive packet configurations */
} UCAN_Config;

/**
  * @brief  Handle structure for the uCAN module.
  * @note   Encapsulates CAN peripheral handle, CAN filter configuration,
  *         node information, packet management, and module status.
  */
typedef struct {
    CAN_HandleTypeDef* hcan;				/*!< Pointer to the STM32 HAL CAN handle */
    CAN_FilterTypeDef filter;				/*!< CAN filter configuration used for message filtering */
    UCAN_NodeInfo node;						/*!< Information about this node and its clients */
    UCAN_PacketHolder txHolder;    			/*!< Container for transmit CAN packets */
    UCAN_PacketHolder rxHolder;				/*!< Container for receive CAN packets */
    UCAN_StatusTypeDef status;				/*!< Current status of the uCAN module */
} UCAN_HandleTypeDef;

#endif
