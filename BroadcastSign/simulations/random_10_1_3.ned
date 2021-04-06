package broadcastsign.simulations;

import inet.common.misc.ThruputMeteringChannel;
import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;
import inet.node.inet.Router;
import inet.node.inet.StandardHost;

network random_10_1_3
            {
            parameters:
            int n;
            types:
            channel fiberline extends ThruputMeteringChannel
            {
            delay = 1us;
            datarate = 512Mbps;
            thruputDisplayFormat = "u";
            }
            channel ethernetline extends ThruputMeteringChannel
            {
            delay = 0.5ms;
            datarate = 1Mbps;
            thruputDisplayFormat = "u";
            }
            submodules:
            configurator: Ipv4NetworkConfigurator {
            parameters:
            @display("p=100,100;is=s");
            }
            cli[n]: StandardHost {
            parameters:
            @display("i=device/laptop"); //;p=250,100,col,100");
            }
            connections:
            cli[0].pppg++ <--> ethernetline <--> cli[1].pppg++;
cli[0].pppg++ <--> ethernetline <--> cli[7].pppg++;
cli[0].pppg++ <--> ethernetline <--> cli[8].pppg++;
cli[1].pppg++ <--> ethernetline <--> cli[2].pppg++;
cli[1].pppg++ <--> ethernetline <--> cli[4].pppg++;
cli[2].pppg++ <--> ethernetline <--> cli[5].pppg++;
cli[2].pppg++ <--> ethernetline <--> cli[9].pppg++;
cli[3].pppg++ <--> ethernetline <--> cli[6].pppg++;
cli[3].pppg++ <--> ethernetline <--> cli[7].pppg++;
cli[3].pppg++ <--> ethernetline <--> cli[8].pppg++;
cli[4].pppg++ <--> ethernetline <--> cli[5].pppg++;
cli[4].pppg++ <--> ethernetline <--> cli[7].pppg++;
cli[5].pppg++ <--> ethernetline <--> cli[6].pppg++;
cli[6].pppg++ <--> ethernetline <--> cli[9].pppg++;
cli[8].pppg++ <--> ethernetline <--> cli[9].pppg++;
}