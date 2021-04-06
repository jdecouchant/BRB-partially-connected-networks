package broadcastsign.simulations;

import inet.common.misc.ThruputMeteringChannel;
import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;
import inet.node.inet.Router;
import inet.node.inet.StandardHost;

network random_25_1_12
			{
			parameters:
			int n;
			types:
			channel fiberline extends ThruputMeteringChannel
			{
			delay = 32us;
			datarate = 250kbps;
			thruputDisplayFormat = "u";
			}
			channel ethernetline extends ThruputMeteringChannel
			{
			delay = 32ms;
			datarate = 250kbps;
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
			cli[0].pppg++ <--> ethernetline <--> cli[2].pppg++;
cli[0].pppg++ <--> ethernetline <--> cli[4].pppg++;
cli[0].pppg++ <--> ethernetline <--> cli[6].pppg++;
cli[0].pppg++ <--> ethernetline <--> cli[7].pppg++;
cli[0].pppg++ <--> ethernetline <--> cli[8].pppg++;
cli[0].pppg++ <--> ethernetline <--> cli[10].pppg++;
cli[0].pppg++ <--> ethernetline <--> cli[11].pppg++;
cli[0].pppg++ <--> ethernetline <--> cli[16].pppg++;
cli[0].pppg++ <--> ethernetline <--> cli[18].pppg++;
cli[0].pppg++ <--> ethernetline <--> cli[20].pppg++;
cli[0].pppg++ <--> ethernetline <--> cli[21].pppg++;
cli[0].pppg++ <--> ethernetline <--> cli[24].pppg++;
cli[1].pppg++ <--> ethernetline <--> cli[2].pppg++;
cli[1].pppg++ <--> ethernetline <--> cli[3].pppg++;
cli[1].pppg++ <--> ethernetline <--> cli[4].pppg++;
cli[1].pppg++ <--> ethernetline <--> cli[7].pppg++;
cli[1].pppg++ <--> ethernetline <--> cli[8].pppg++;
cli[1].pppg++ <--> ethernetline <--> cli[12].pppg++;
cli[1].pppg++ <--> ethernetline <--> cli[15].pppg++;
cli[1].pppg++ <--> ethernetline <--> cli[16].pppg++;
cli[1].pppg++ <--> ethernetline <--> cli[19].pppg++;
cli[1].pppg++ <--> ethernetline <--> cli[20].pppg++;
cli[1].pppg++ <--> ethernetline <--> cli[22].pppg++;
cli[1].pppg++ <--> ethernetline <--> cli[23].pppg++;
cli[2].pppg++ <--> ethernetline <--> cli[3].pppg++;
cli[2].pppg++ <--> ethernetline <--> cli[7].pppg++;
cli[2].pppg++ <--> ethernetline <--> cli[8].pppg++;
cli[2].pppg++ <--> ethernetline <--> cli[10].pppg++;
cli[2].pppg++ <--> ethernetline <--> cli[11].pppg++;
cli[2].pppg++ <--> ethernetline <--> cli[17].pppg++;
cli[2].pppg++ <--> ethernetline <--> cli[18].pppg++;
cli[2].pppg++ <--> ethernetline <--> cli[19].pppg++;
cli[2].pppg++ <--> ethernetline <--> cli[23].pppg++;
cli[2].pppg++ <--> ethernetline <--> cli[24].pppg++;
cli[3].pppg++ <--> ethernetline <--> cli[4].pppg++;
cli[3].pppg++ <--> ethernetline <--> cli[5].pppg++;
cli[3].pppg++ <--> ethernetline <--> cli[6].pppg++;
cli[3].pppg++ <--> ethernetline <--> cli[7].pppg++;
cli[3].pppg++ <--> ethernetline <--> cli[11].pppg++;
cli[3].pppg++ <--> ethernetline <--> cli[17].pppg++;
cli[3].pppg++ <--> ethernetline <--> cli[18].pppg++;
cli[3].pppg++ <--> ethernetline <--> cli[20].pppg++;
cli[3].pppg++ <--> ethernetline <--> cli[22].pppg++;
cli[3].pppg++ <--> ethernetline <--> cli[23].pppg++;
cli[4].pppg++ <--> ethernetline <--> cli[5].pppg++;
cli[4].pppg++ <--> ethernetline <--> cli[6].pppg++;
cli[4].pppg++ <--> ethernetline <--> cli[9].pppg++;
cli[4].pppg++ <--> ethernetline <--> cli[10].pppg++;
cli[4].pppg++ <--> ethernetline <--> cli[12].pppg++;
cli[4].pppg++ <--> ethernetline <--> cli[14].pppg++;
cli[4].pppg++ <--> ethernetline <--> cli[18].pppg++;
cli[4].pppg++ <--> ethernetline <--> cli[20].pppg++;
cli[4].pppg++ <--> ethernetline <--> cli[24].pppg++;
cli[5].pppg++ <--> ethernetline <--> cli[11].pppg++;
cli[5].pppg++ <--> ethernetline <--> cli[12].pppg++;
cli[5].pppg++ <--> ethernetline <--> cli[14].pppg++;
cli[5].pppg++ <--> ethernetline <--> cli[15].pppg++;
cli[5].pppg++ <--> ethernetline <--> cli[16].pppg++;
cli[5].pppg++ <--> ethernetline <--> cli[17].pppg++;
cli[5].pppg++ <--> ethernetline <--> cli[18].pppg++;
cli[5].pppg++ <--> ethernetline <--> cli[20].pppg++;
cli[5].pppg++ <--> ethernetline <--> cli[22].pppg++;
cli[5].pppg++ <--> ethernetline <--> cli[24].pppg++;
cli[6].pppg++ <--> ethernetline <--> cli[7].pppg++;
cli[6].pppg++ <--> ethernetline <--> cli[8].pppg++;
cli[6].pppg++ <--> ethernetline <--> cli[9].pppg++;
cli[6].pppg++ <--> ethernetline <--> cli[10].pppg++;
cli[6].pppg++ <--> ethernetline <--> cli[13].pppg++;
cli[6].pppg++ <--> ethernetline <--> cli[14].pppg++;
cli[6].pppg++ <--> ethernetline <--> cli[15].pppg++;
cli[6].pppg++ <--> ethernetline <--> cli[16].pppg++;
cli[6].pppg++ <--> ethernetline <--> cli[19].pppg++;
cli[7].pppg++ <--> ethernetline <--> cli[10].pppg++;
cli[7].pppg++ <--> ethernetline <--> cli[11].pppg++;
cli[7].pppg++ <--> ethernetline <--> cli[12].pppg++;
cli[7].pppg++ <--> ethernetline <--> cli[13].pppg++;
cli[7].pppg++ <--> ethernetline <--> cli[19].pppg++;
cli[7].pppg++ <--> ethernetline <--> cli[21].pppg++;
cli[7].pppg++ <--> ethernetline <--> cli[24].pppg++;
cli[8].pppg++ <--> ethernetline <--> cli[10].pppg++;
cli[8].pppg++ <--> ethernetline <--> cli[11].pppg++;
cli[8].pppg++ <--> ethernetline <--> cli[12].pppg++;
cli[8].pppg++ <--> ethernetline <--> cli[14].pppg++;
cli[8].pppg++ <--> ethernetline <--> cli[16].pppg++;
cli[8].pppg++ <--> ethernetline <--> cli[18].pppg++;
cli[8].pppg++ <--> ethernetline <--> cli[21].pppg++;
cli[8].pppg++ <--> ethernetline <--> cli[22].pppg++;
cli[9].pppg++ <--> ethernetline <--> cli[12].pppg++;
cli[9].pppg++ <--> ethernetline <--> cli[13].pppg++;
cli[9].pppg++ <--> ethernetline <--> cli[14].pppg++;
cli[9].pppg++ <--> ethernetline <--> cli[15].pppg++;
cli[9].pppg++ <--> ethernetline <--> cli[16].pppg++;
cli[9].pppg++ <--> ethernetline <--> cli[17].pppg++;
cli[9].pppg++ <--> ethernetline <--> cli[18].pppg++;
cli[9].pppg++ <--> ethernetline <--> cli[20].pppg++;
cli[9].pppg++ <--> ethernetline <--> cli[22].pppg++;
cli[9].pppg++ <--> ethernetline <--> cli[23].pppg++;
cli[10].pppg++ <--> ethernetline <--> cli[13].pppg++;
cli[10].pppg++ <--> ethernetline <--> cli[14].pppg++;
cli[10].pppg++ <--> ethernetline <--> cli[18].pppg++;
cli[10].pppg++ <--> ethernetline <--> cli[19].pppg++;
cli[10].pppg++ <--> ethernetline <--> cli[21].pppg++;
cli[10].pppg++ <--> ethernetline <--> cli[24].pppg++;
cli[11].pppg++ <--> ethernetline <--> cli[12].pppg++;
cli[11].pppg++ <--> ethernetline <--> cli[15].pppg++;
cli[11].pppg++ <--> ethernetline <--> cli[18].pppg++;
cli[11].pppg++ <--> ethernetline <--> cli[19].pppg++;
cli[11].pppg++ <--> ethernetline <--> cli[21].pppg++;
cli[11].pppg++ <--> ethernetline <--> cli[23].pppg++;
cli[12].pppg++ <--> ethernetline <--> cli[13].pppg++;
cli[12].pppg++ <--> ethernetline <--> cli[16].pppg++;
cli[12].pppg++ <--> ethernetline <--> cli[19].pppg++;
cli[12].pppg++ <--> ethernetline <--> cli[21].pppg++;
cli[12].pppg++ <--> ethernetline <--> cli[24].pppg++;
cli[13].pppg++ <--> ethernetline <--> cli[14].pppg++;
cli[13].pppg++ <--> ethernetline <--> cli[15].pppg++;
cli[13].pppg++ <--> ethernetline <--> cli[16].pppg++;
cli[13].pppg++ <--> ethernetline <--> cli[17].pppg++;
cli[13].pppg++ <--> ethernetline <--> cli[22].pppg++;
cli[13].pppg++ <--> ethernetline <--> cli[23].pppg++;
cli[13].pppg++ <--> ethernetline <--> cli[24].pppg++;
cli[14].pppg++ <--> ethernetline <--> cli[19].pppg++;
cli[14].pppg++ <--> ethernetline <--> cli[21].pppg++;
cli[14].pppg++ <--> ethernetline <--> cli[22].pppg++;
cli[14].pppg++ <--> ethernetline <--> cli[23].pppg++;
cli[14].pppg++ <--> ethernetline <--> cli[24].pppg++;
cli[15].pppg++ <--> ethernetline <--> cli[17].pppg++;
cli[15].pppg++ <--> ethernetline <--> cli[19].pppg++;
cli[15].pppg++ <--> ethernetline <--> cli[20].pppg++;
cli[15].pppg++ <--> ethernetline <--> cli[21].pppg++;
cli[15].pppg++ <--> ethernetline <--> cli[23].pppg++;
cli[15].pppg++ <--> ethernetline <--> cli[24].pppg++;
cli[16].pppg++ <--> ethernetline <--> cli[17].pppg++;
cli[16].pppg++ <--> ethernetline <--> cli[18].pppg++;
cli[16].pppg++ <--> ethernetline <--> cli[20].pppg++;
cli[16].pppg++ <--> ethernetline <--> cli[21].pppg++;
cli[17].pppg++ <--> ethernetline <--> cli[19].pppg++;
cli[17].pppg++ <--> ethernetline <--> cli[20].pppg++;
cli[17].pppg++ <--> ethernetline <--> cli[21].pppg++;
cli[17].pppg++ <--> ethernetline <--> cli[22].pppg++;
cli[17].pppg++ <--> ethernetline <--> cli[24].pppg++;
cli[18].pppg++ <--> ethernetline <--> cli[23].pppg++;
cli[18].pppg++ <--> ethernetline <--> cli[24].pppg++;
cli[19].pppg++ <--> ethernetline <--> cli[22].pppg++;
cli[19].pppg++ <--> ethernetline <--> cli[23].pppg++;
cli[20].pppg++ <--> ethernetline <--> cli[21].pppg++;
cli[20].pppg++ <--> ethernetline <--> cli[22].pppg++;
cli[20].pppg++ <--> ethernetline <--> cli[23].pppg++;
cli[21].pppg++ <--> ethernetline <--> cli[22].pppg++;
cli[22].pppg++ <--> ethernetline <--> cli[23].pppg++;
}