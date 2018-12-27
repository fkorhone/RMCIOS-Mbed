/* 2015 Frans Korhonen. University of Helsinki.
 *
 */
#ifndef mbed_ethernet_channels_h
#define mbed_ethernet_channels_h

#if DEVICE_ETHERNET == 1
#include "EthernetInterface.h"
#include "RMCIOS-functions.h"

extern const struct context_rmcios *module_context ; 

//////////////////////////////////////////////////////////////////////
// Ethernet interface channel
//////////////////////////////////////////////////////////////////////
EthernetInterface enet ;
EthernetInterface *eth=&enet ;
char eth_configured=0;

void ethernet_class_func(const struct context_rmcios *context, 
                         void *data, int id, int function,
                         enum type_rmcios paramtype,union param_rmcios returnv, 
                         int num_params,union param_rmcios param)
{
    int iplen ;
    int masklen ;
    int gwlen ;

    switch(function) {
        case help_rmcios:
            return_string(context, paramtype,returnv,
            "help for ethernet interface channel\r\n"
            " setup eth # Configure ethernet with DHCP\r\n"
            " setup eth ip_addr ipmask gateway # Manual configuration\r\n"
            ) ;
            break ;

        case setup_rmcios :
            if(eth==NULL) eth = new EthernetInterface ; // Allocate ethernet interface.
            if(eth==NULL) break ;
            if(eth_configured==1) {
                return_string(context, paramtype,returnv,"Ethernet can only be configured once.") ;
                break ;
            }

            if(num_params<3) {
                return_string(context, paramtype,returnv,"Configuring ethernet with DHCP:") ;
                eth_configured=1;
                eth->init() ; // dhcp config
                eth->connect();
                
                return_string(context, paramtype,returnv,"Ethernet configuration:\r\n") ;
                return_string(context, paramtype,returnv,"ip:");
                return_string(context, paramtype,returnv,eth->getIPAddress());
                return_string(context, paramtype,returnv,"\r\nipmask:");
                return_string(context, paramtype,returnv,eth->getNetworkMask());
                return_string(context, paramtype,returnv,"\r\ngateway:");
                return_string(context, paramtype,returnv,eth->getGateway());
                return_string(context, paramtype,returnv,"\r\n");
                break ;
            }

            iplen=param_string_length(context, paramtype,param,0)+1 ;
            masklen=param_string_length(context, paramtype,param,1)+1 ;
            gwlen=param_string_length(context, paramtype,param,2)+1 ;
            {

                char ip[iplen] ;
                char ipmask[masklen] ;
                char gateway[gwlen] ;
                param_to_string(context, paramtype, param, 0,iplen,ip) ;
                param_to_string(context, paramtype, param, 1,masklen,ipmask) ;
                param_to_string(context, paramtype, param, 2,gwlen,gateway) ;

                printf("setup manual ip: %s %s %s\r\n",ip,ipmask,gateway) ;
                eth_configured=1;
                eth->init(ip,ipmask,gateway) ; // manual ip config
                eth->connect();

                return_string(context, paramtype,returnv,"Ethernet configuration:\r\n") ;
                return_string(context, paramtype,returnv,"ip:");
                return_string(context, paramtype,returnv,eth->getIPAddress());
                return_string(context, paramtype,returnv,"\r\nipmask:");
                return_string(context, paramtype,returnv,eth->getNetworkMask());
                return_string(context, paramtype,returnv,"\r\ngateway:");
                return_string(context, paramtype,returnv,eth->getGateway());
                return_string(context, paramtype,returnv,"\r\n");
            }
            break ;
    }
}

////////////////////////////////////////////////////////////////////////
// TCP Client channel:
////////////////////////////////////////////////////////////////////////
struct tcp_connection_data {
    char host[50] ;
    int port ;
    TCPSocketConnection socket;
    bool closed ;
};

void tcp_client_class_func(const struct context_rmcios *context, 
                           void* data,int id, int function,
                           enum type_rmcios paramtype,union param_rmcios returnv, 
                           int num_params,union param_rmcios param)
{
    tcp_connection_data *p_data=(tcp_connection_data *)data ;
    int plen ;
    switch(function) {
        case help_rmcios:
           return_string(context, paramtype, returnv,
           "help for tcp client channel\r\n"
           " create tcp_client newname\r\n"
           " setup newname ip port #Open connection\r\n"
           " setup newname #Close connection\r\n"
           " write newname #Flush all buffers\r\n"
           " write newname data #Write data to the connection. Reconnect if needed\r\n"
           " link newname channel # Link received data to channel\r\n"
           ) ;
            break ;
        case create_rmcios :
            if(num_params<1) break ;
            if(eth==NULL) break ;

            p_data= new struct tcp_connection_data ; // allocate new data

            // default values
            p_data->host[0]=0;
            p_data->port=0;
            p_data->closed=true ;

            create_channel_param(context, paramtype, param, 0, 
                                 (class_rmcios)tcp_client_class_func, p_data) ; 
                                 
            break ;
        case setup_rmcios :
            if(p_data==NULL) break ;
            if(num_params<1) { // Close connection
                Thread::wait(500);
                if(p_data==NULL) break ;
                if(p_data->socket.is_connected()) p_data->socket.close();
                break ;
            }
            if(num_params<2) break ;
            param_to_string(context, paramtype, param, 0, sizeof(p_data->host),p_data->host) ;
            p_data->port=param_to_int(context, paramtype,param,1) ;
            p_data->closed=true ;
            //p_data->socket.connect(p_data->host,p_data->port);
            //if(p_data->socket.is_connected()) p_data->socket.close();
            break ;
        case write_rmcios :
            if(p_data==NULL) break ;
            if(num_params<1) { // Close connection
                Thread::wait(500);
                if(p_data==NULL) break ;
                if(p_data->socket.is_connected()) p_data->socket.close();
                break ;
            }

            /// If socket is not open. Try 3 times to open connection:
            if(!p_data->socket.is_connected() || p_data->closed==true ) {
                if(p_data->socket.connect(p_data->host,p_data->port)==0) p_data->closed=false ;
                else p_data->closed=false ;
            }
            if(!p_data->socket.is_connected() || p_data->closed==true ) {
                Thread::wait(200);
                if(p_data->socket.connect(p_data->host,p_data->port)==0) p_data->closed=false ;
                else p_data->closed=false ;
            }
            if(!p_data->socket.is_connected() || p_data->closed==true ) {
                Thread::wait(200);
                if(p_data->socket.connect(p_data->host,p_data->port)==0) p_data->closed=false ;
                else p_data->closed=false ;
            }

            plen=param_buffer_alloc_size(context, paramtype,param,0) ;
            {
                char buffer[plen] ;
                int retry;
                struct buffer_rmcios p ;
                p=param_to_buffer(context, paramtype,param, 0, plen, buffer) ;
                for(retry=0 ;
                        retry<3 && p_data->socket.send(p.data , p.length )<0 ;
                        retry++ ) // retry 3 times
                    Thread::wait(200); // Wait for socket to send properly.
                Thread::wait(100);
            }
            break ;
    }
}
///////////////////////////////////////////////////////
// TCP server channel
////////////////////////////////////////////////////////
struct tcpserver_data {
    int port ;
    TCPSocketServer socket ;
    TCPSocketConnection connection;
    int linked_channels ;
    char buffer[512] ;
    int id ;
};

//! Thread for handling udp reception.
void tcpserver_thread(const void *param)
{
    printf("tcp server thread created!\r\n") ;
    int n=0 ;
    struct tcpserver_data *pthis=(struct tcpserver_data *) param ;
    
    while(pthis->port==0) ;
    {
        Thread::wait(200);
    }
    
    pthis->socket.bind(pthis->port) ;
    pthis->socket.listen();
    
    while(1) {
        pthis->socket.accept( pthis->connection )  ;
        printf("connection accepted!\r\n") ;
        // receive command
        while( pthis->connection.is_connected() ) {
            n=pthis->connection.receive( pthis->buffer, sizeof( pthis->buffer)-1 ) ;
            if(n>0) {
                if( pthis->linked_channels!=0) write_buffer(module_context, pthis->linked_channels,pthis->buffer,n,pthis->id) ;
            }
        }
    }
}

void tcp_server_class_func(const struct context_rmcios *context,
                           struct tcpserver_data *pthis, int id, int function,
                           enum type_rmcios paramtype,union param_rmcios returnv, 
                           int num_params,union param_rmcios param)
{
    int plen;
    switch(function) {
        case help_rmcios:
            return_string(context, paramtype, returnv, 
                          "TCP server channel\r\n"
                          "create tcpserver newname\r\n"
                          "setup newname port\r\n"
                          "write newname data\r\n"
                          "link newname channel\r\n"
                          );
            break ;

        case create_rmcios :
            if(num_params<1) break ;
            pthis=  (struct tcpserver_data *) malloc(sizeof( struct tcpserver_data)) ; // allocate new data
            
            //default values :
            pthis->port=0 ;
            
            pthis->buffer[0]=0;
            pthis->socket=TCPSocketServer() ;
            pthis->connection=TCPSocketConnection() ;
            pthis->id=create_channel_param(context, paramtype, param, 0,
                                           (class_rmcios)tcp_server_class_func, pthis) ; 
            pthis->linked_channels=linked_channels(context, pthis->id) ;
            break;

        case setup_rmcios :
            if(pthis==NULL) break ;
            if(num_params<1) {
                if( !pthis->connection.is_connected() ) pthis->connection.close() ;
                break ;
            }
            pthis->port=param_to_integer(context, paramtype, param,0);
            
            // Start reception thread
            new Thread(tcpserver_thread, pthis);
            break;

        case write_rmcios :
            if(pthis==NULL) break ;
            if(num_params<1) break ;
            // Determine the needed buffer size
            plen=param_buffer_alloc_size(context, paramtype, param, 0) ; 
            {
                char buffer[plen] ; // allocate buffer
                struct buffer_rmcios pbuffer ; // structure pointer to buffer data
                pbuffer = param_to_buffer(context, paramtype,param, 0, plen , buffer) ;
                if(pthis->connection.send(pbuffer.data, pbuffer.length)<0){
                    return_int(context, paramtype, returnv, -1) ;
                    pthis->connection.close() ;
                }
            }
            break;
    }
}

///////////////////////////////////////////////////////
// UDP server channel
////////////////////////////////////////////////////////
struct udp_server_data {
    int id ;
    int port ;
    UDPSocket socket ;
    Endpoint client ;
    char txbuffer[100] ;
    char buffer[100] ;
    int linked_channel ;
} ;

//! Thread for handling udp reception.
void udp_server_thread(const void *param)
{
    //printf("udp thread created!\r\n") ;
    int n=0 ;
    struct udp_server_data *p_data=(struct udp_server_data *) param ;
    //printf("waiting for configuration:\r\n") ;
    while(p_data->port==0) ;
    {
        p_data->socket.bind(p_data->port) ;
        Thread::wait(200);
    }
    while(1) {
        // receive command
        n=p_data->socket.receiveFrom(p_data->client, p_data->buffer, sizeof(p_data->buffer)-1 ) ;
        if(n>0) {
            if(p_data->linked_channel!=0) write_buffer(module_context, p_data->linked_channel, p_data->buffer, n, p_data->id) ;
        }
    }
}

void udp_server_class_func(const struct context_rmcios *context, 
                           udp_server_data *p_data,  int id, int function,
                           enum type_rmcios paramtype,union param_rmcios returnv, 
                           int num_params,union param_rmcios param)
{
    switch(function) {
        case help_rmcios :
            return_string(context, paramtype, returnv,
            "help for udp server channel\r\n"
            " create udp_server newname\r\n"
            " setup newname port \r\n"
            " write newname # flush tansmit buffer)\r\n"
            " write newname data # write data to tansmit buffer. Flush buffer to ethernet when \\r encountered. \r\n"
            " link newname channel # Link received data to channel\r\n"
            ) ;

            break ;
        case create_rmcios :
            if(num_params<1) break ;
            if(eth==NULL) break ;

            p_data= new struct udp_server_data ; // allocate new data
            if(p_data==NULL) break ;

            // Default values:
            p_data->port=0;
            p_data->buffer[0]=0;
            p_data->txbuffer[0]=0 ;
            

            // Create the channel
            p_data->id=create_channel_param(context, paramtype, param, 0, (class_rmcios)udp_server_class_func, p_data) ;
            p_data->linked_channel=linked_channels(context,p_data->id) ;
            break ;
        case setup_rmcios :

            if(p_data==NULL) break ;
            if(num_params<1) break ;
            else {
                p_data->port=param_to_int(context, paramtype, param, 0) ;

                // Create receive thread
                new Thread(udp_server_thread, p_data);
            }
            break ;

        case write_rmcios :
            if(p_data==NULL) break ;
            if(num_params<1) { // flush buffer
                p_data->socket.sendTo(p_data->client, p_data->txbuffer , strlen(p_data->txbuffer));
                p_data->txbuffer[0]=0 ;
                break ;
            }

            // collect data to buffer
            param_to_string(context, paramtype, param, 0, sizeof(p_data->txbuffer)-strlen(p_data->txbuffer), p_data->txbuffer+strlen(p_data->txbuffer)) ;
            break ;
    }
}

void init_mbed_ethernet_channels(const struct context_rmcios *context)
{
    module_context=context ;
    create_channel_str(context, "eth",(class_rmcios)ethernet_class_func, NULL ) ;
    create_channel_str(context, "udp_server",(class_rmcios)udp_server_class_func, NULL ) ;
    // TCP Not usable. (Memory consumption issues.)
    // create_channel_str("tcp_client",(channel_func) tcp_client_class_func, NULL ) ;
    // create_channel_str("tcp_server",(channel_func) tcp_server_class_func, NULL ) ;
}
#endif

#endif

