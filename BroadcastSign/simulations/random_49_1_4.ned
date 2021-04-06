package broadcastsign.simulations;

import inet.common.misc.ThruputMeteringChannel;
import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;
import inet.node.inet.Router;
import inet.node.inet.StandardHost;

network random_49_1_4
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
			cli[0].pppg++ <--> ethernetline <--> cli[11].pppg++;
cli[0].pppg++ <--> ethernetline <--> cli[21].pppg++;
cli[0].pppg++ <--> ethernetline <--> cli[22].pppg++;
cli[0].pppg++ <--> ethernetline <--> cli[40].pppg++;
cli[1].pppg++ <--> ethernetline <--> cli[4].pppg++;
cli[1].pppg++ <--> ethernetline <--> cli[12].pppg++;
cli[1].pppg++ <--> ethernetline <--> cli[16].pppg++;
cli[1].pppg++ <--> ethernetline <--> cli[29].pppg++;
cli[2].pppg++ <--> ethernetline <--> cli[22].pppg++;
cli[2].pppg++ <--> ethernetline <--> cli[31].pppg++;
cli[2].pppg++ <--> ethernetline <--> cli[34].pppg++;
cli[2].pppg++ <--> ethernetline <--> cli[41].pppg++;
cli[3].pppg++ <--> ethernetline <--> cli[12].pppg++;
cli[3].pppg++ <--> ethernetline <--> cli[17].pppg++;
cli[3].pppg++ <--> ethernetline <--> cli[24].pppg++;
cli[3].pppg++ <--> ethernetline <--> cli[27].pppg++;
cli[4].pppg++ <--> ethernetline <--> cli[25].pppg++;
cli[4].pppg++ <--> ethernetline <--> cli[37].pppg++;
cli[4].pppg++ <--> ethernetline <--> cli[40].pppg++;
cli[5].pppg++ <--> ethernetline <--> cli[10].pppg++;
cli[5].pppg++ <--> ethernetline <--> cli[14].pppg++;
cli[5].pppg++ <--> ethernetline <--> cli[30].pppg++;
cli[5].pppg++ <--> ethernetline <--> cli[33].pppg++;
cli[6].pppg++ <--> ethernetline <--> cli[11].pppg++;
cli[6].pppg++ <--> ethernetline <--> cli[14].pppg++;
cli[6].pppg++ <--> ethernetline <--> cli[20].pppg++;
cli[6].pppg++ <--> ethernetline <--> cli[33].pppg++;
cli[7].pppg++ <--> ethernetline <--> cli[13].pppg++;
cli[7].pppg++ <--> ethernetline <--> cli[15].pppg++;
cli[7].pppg++ <--> ethernetline <--> cli[31].pppg++;
cli[7].pppg++ <--> ethernetline <--> cli[41].pppg++;
cli[8].pppg++ <--> ethernetline <--> cli[9].pppg++;
cli[8].pppg++ <--> ethernetline <--> cli[20].pppg++;
cli[8].pppg++ <--> ethernetline <--> cli[23].pppg++;
cli[8].pppg++ <--> ethernetline <--> cli[47].pppg++;
cli[9].pppg++ <--> ethernetline <--> cli[20].pppg++;
cli[9].pppg++ <--> ethernetline <--> cli[22].pppg++;
cli[9].pppg++ <--> ethernetline <--> cli[40].pppg++;
cli[10].pppg++ <--> ethernetline <--> cli[28].pppg++;
cli[10].pppg++ <--> ethernetline <--> cli[32].pppg++;
cli[10].pppg++ <--> ethernetline <--> cli[43].pppg++;
cli[11].pppg++ <--> ethernetline <--> cli[19].pppg++;
cli[11].pppg++ <--> ethernetline <--> cli[44].pppg++;
cli[12].pppg++ <--> ethernetline <--> cli[36].pppg++;
cli[12].pppg++ <--> ethernetline <--> cli[39].pppg++;
cli[13].pppg++ <--> ethernetline <--> cli[18].pppg++;
cli[13].pppg++ <--> ethernetline <--> cli[24].pppg++;
cli[13].pppg++ <--> ethernetline <--> cli[48].pppg++;
cli[14].pppg++ <--> ethernetline <--> cli[18].pppg++;
cli[14].pppg++ <--> ethernetline <--> cli[30].pppg++;
cli[15].pppg++ <--> ethernetline <--> cli[20].pppg++;
cli[15].pppg++ <--> ethernetline <--> cli[25].pppg++;
cli[15].pppg++ <--> ethernetline <--> cli[47].pppg++;
cli[16].pppg++ <--> ethernetline <--> cli[27].pppg++;
cli[16].pppg++ <--> ethernetline <--> cli[36].pppg++;
cli[16].pppg++ <--> ethernetline <--> cli[45].pppg++;
cli[17].pppg++ <--> ethernetline <--> cli[24].pppg++;
cli[17].pppg++ <--> ethernetline <--> cli[31].pppg++;
cli[17].pppg++ <--> ethernetline <--> cli[39].pppg++;
cli[18].pppg++ <--> ethernetline <--> cli[21].pppg++;
cli[18].pppg++ <--> ethernetline <--> cli[45].pppg++;
cli[19].pppg++ <--> ethernetline <--> cli[26].pppg++;
cli[19].pppg++ <--> ethernetline <--> cli[36].pppg++;
cli[19].pppg++ <--> ethernetline <--> cli[47].pppg++;
cli[21].pppg++ <--> ethernetline <--> cli[29].pppg++;
cli[21].pppg++ <--> ethernetline <--> cli[42].pppg++;
cli[22].pppg++ <--> ethernetline <--> cli[26].pppg++;
cli[23].pppg++ <--> ethernetline <--> cli[27].pppg++;
cli[23].pppg++ <--> ethernetline <--> cli[30].pppg++;
cli[23].pppg++ <--> ethernetline <--> cli[35].pppg++;
cli[24].pppg++ <--> ethernetline <--> cli[34].pppg++;
cli[25].pppg++ <--> ethernetline <--> cli[37].pppg++;
cli[25].pppg++ <--> ethernetline <--> cli[48].pppg++;
cli[26].pppg++ <--> ethernetline <--> cli[38].pppg++;
cli[26].pppg++ <--> ethernetline <--> cli[44].pppg++;
cli[27].pppg++ <--> ethernetline <--> cli[29].pppg++;
cli[28].pppg++ <--> ethernetline <--> cli[32].pppg++;
cli[28].pppg++ <--> ethernetline <--> cli[35].pppg++;
cli[28].pppg++ <--> ethernetline <--> cli[46].pppg++;
cli[29].pppg++ <--> ethernetline <--> cli[40].pppg++;
cli[30].pppg++ <--> ethernetline <--> cli[48].pppg++;
cli[31].pppg++ <--> ethernetline <--> cli[48].pppg++;
cli[32].pppg++ <--> ethernetline <--> cli[42].pppg++;
cli[32].pppg++ <--> ethernetline <--> cli[46].pppg++;
cli[33].pppg++ <--> ethernetline <--> cli[38].pppg++;
cli[33].pppg++ <--> ethernetline <--> cli[41].pppg++;
cli[34].pppg++ <--> ethernetline <--> cli[42].pppg++;
cli[34].pppg++ <--> ethernetline <--> cli[46].pppg++;
cli[35].pppg++ <--> ethernetline <--> cli[38].pppg++;
cli[35].pppg++ <--> ethernetline <--> cli[41].pppg++;
cli[36].pppg++ <--> ethernetline <--> cli[42].pppg++;
cli[37].pppg++ <--> ethernetline <--> cli[45].pppg++;
cli[37].pppg++ <--> ethernetline <--> cli[46].pppg++;
cli[38].pppg++ <--> ethernetline <--> cli[45].pppg++;
cli[39].pppg++ <--> ethernetline <--> cli[43].pppg++;
cli[39].pppg++ <--> ethernetline <--> cli[44].pppg++;
cli[43].pppg++ <--> ethernetline <--> cli[44].pppg++;
cli[43].pppg++ <--> ethernetline <--> cli[47].pppg++;
}