import inet.common.INETDefs;
import inet.common.packet.chunk.Chunk;
import inet.networklayer.contract.ipv4.Ipv4Address;

namespace inet;

class BriefPacket extends FieldsChunk {
        // Determines the size of the packet on the links 
        // !! To be manually set after the message has been created, depending on the fields that are used !!
        chunkLength = B(1); 
        
        int msgType; // For Bracha's algorithm (SEND, ECHO or READY)	
        
        // Identifiers of a broadcast message
        int broadcasterId; // Source of the message
        int msgId; // Msg ID determined by the source
        
        int echoOrReadySender;
        
        int path[];  // For Dolev's algorithm
        
        int linkSenderId; // Always used to determine who sent a message (in theory should be done by the authenticated link)
        
        int ackedEchoForREADY_ECHO; // Only for the Ready2 messages

		bool includeData;
		
		int dataSize;
		char data[];

		int debugId; 
}

//class SendPacket extends FieldsChunk {
//    chunkLength = B(1);
//    int linkSenderId; // Used to determine who sent a msg, for ease (in theory should be done by the authenticated link)
//    
//	int msgType; // The msg type (SEND). Could be removed and only taken into account for the chunkLength to relieve the simulator a little bit. 
//	
////	int broadcasterId; // Id of the broadcasting process (not needed with Send messages)
//	int msgId; // Local counter 
//	
////	int localPayloadId;  // One ID per (broadcasterId, msgId) broadcast // Used only for network consumption
////	int payloadSize; // Number of bytes the payload contains // Used only for network consumption
////	byte payload[]; // Voluntarily commented: chunkLength manually set in the code
//}


