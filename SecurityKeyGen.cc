#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/mobility-model.h"
#include "ns3/position-allocator.h"
#include "ns3/mobility-helper.h"
#include "ns3/netanim-module.h"
#include "ns3/ptr.h"
#include "ns3/packet.h"
#include "ns3/header.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("FirstScriptExample");
// instantiate a packet
   Ptr<Packet> p = Create<Packet> ();
   uint16_t add,D1;
class MyHeader : public Header 
 {
 public:
 
   MyHeader ();
   virtual ~MyHeader ();
 
   void SetData (uint16_t data);
   uint16_t GetData (void) const;
 
   static TypeId GetTypeId (void);
   virtual TypeId GetInstanceTypeId (void) const;
   virtual void Print (std::ostream &os) const;
   virtual void Serialize (Buffer::Iterator start) const;
   virtual uint32_t Deserialize (Buffer::Iterator start);
   virtual uint32_t GetSerializedSize (void) const;
   private:
   uint16_t m_data;  
 };
 
 MyHeader::MyHeader ()
 {
   // we must provide a public default constructor, 
   // implicit or explicit, but never private.
 }
 MyHeader::~MyHeader ()
 {
 }
 
 TypeId
 MyHeader::GetTypeId (void)
 {
   static TypeId tid = TypeId ("ns3::MyHeader")
     .SetParent<Header> ()
     .AddConstructor<MyHeader> ()
   ;
   return tid;
 }
 TypeId
 MyHeader::GetInstanceTypeId (void) const
 {
   return GetTypeId ();
 }
 
 void
 MyHeader::Print (std::ostream &os) const
 {
   // This method is invoked by the packet printing
   // routines to print the content of my header.
   //os << "data=" << m_data << std::endl;
   os << "data=" << m_data << std::endl;
 }
 uint32_t
 MyHeader::GetSerializedSize (void) const
 {
   // we reserve 2 bytes for our header.
   return 2;
 }
 void
 MyHeader::Serialize (Buffer::Iterator start) const
 {
   // we can serialize two bytes at the start of the buffer.
   // we write them in network byte order.
   start.WriteHtonU16 (m_data);
 }
 uint32_t
 MyHeader::Deserialize (Buffer::Iterator start)
 {
   // we can deserialize two bytes from the start of the buffer.
   // we read them in network byte order and store them
   // in host byte order.
   m_data = start.ReadNtohU16 ();
 
   // we return the number of bytes effectively read.
   return 2;
 }
 
 void 
 MyHeader::SetData (uint16_t data)
 {
   m_data = data;
 }
 uint16_t 
 MyHeader::GetData (void) const
 {
   return m_data;
 }
 
 static uint16_t keygen(uint16_t data)
 {
 uint16_t sum=0,r=0;
 while(data>0)
      {
      r=data%10;
      sum=sum+r;
      data=data/10;       
      }           
     std::cout << "security key by node:"<< sum << std::endl; 
     return sum;
 }
void ReceivePacket (Ptr<Socket> socket)
{
  MyHeader destinationHeader;
  p -> RemoveHeader(destinationHeader);
  while ( socket -> Recv() )
  {
  std::cout << "Received Data Packet by node:"<< destinationHeader.GetData ()<<std::endl;
      D1 = destinationHeader.GetData();          
  }
  //Call key generation method
      add= keygen(D1);
}
    
/**
 * Geerate traffic
 * \param socket Tx socket
 * \param pktSize packet size
 * \param pktCount number of packets
 * \param pktInterval interval between packet generation
 */
static void GenerateTraffic (Ptr<Socket> socket, Ptr<Packet> pkt, uint32_t pktSize,
                             uint32_t pktCount, Time pktInterval )
{

   // print the content of my packet on the standard output.
   
  if (pktCount > 0)
    {
    std::cout << "Sent Packet:";
    pkt->Print (std::cout);
   std::cout << std::endl;
            socket->Send (pkt);
      Simulator::Schedule (pktInterval, &GenerateTraffic,
                           socket, pkt, pktSize,pktCount - 1, pktInterval);
    }
  else
    {
      socket->Close ();
    }
}

int main (int argc, char *argv[])
{

uint32_t packetSize = 1024; // bytes
  uint32_t numPackets = 1;
  double interval = 1.0; // seconds
  bool verbose = false;
  CommandLine cmd (__FILE__);
  //cmd.AddValue ("phyMode", "Wifi Phy mode", phyMode);
  cmd.AddValue ("packetSize", "size of application packet sent", packetSize);
  cmd.AddValue ("numPackets", "number of packets generated", numPackets);
  cmd.AddValue ("interval", "interval (seconds) between packets", interval);
  cmd.AddValue ("verbose", "turn on all WifiNetDevice log components", verbose);
  cmd.Parse (argc, argv);
  // Convert to time object
  Time interPacketInterval = Seconds (interval);

// Enable the packet printing through Packet::Print command.
   Packet::EnablePrinting ();
   
  Time::SetResolution (Time::NS);
  LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
  LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);

  NS_LOG_INFO ("Create nodes.");
  NodeContainer terminals;
  terminals.Create (4);
  
  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));

  NetDeviceContainer terminalDevices = pointToPoint.Install (NodeContainer (terminals.Get (0), terminals.Get(1)));
  NetDeviceContainer terminalDevices1 = pointToPoint.Install (NodeContainer (terminals.Get (0), terminals.Get(2)));
  NetDeviceContainer terminalDevices2 = pointToPoint.Install (NodeContainer (terminals.Get (0), terminals.Get(3)));

   //Tracing
  pointToPoint.EnablePcap ("P2P", terminalDevices);
  pointToPoint.EnablePcap ("P2P1", terminalDevices1);
  pointToPoint.EnablePcap ("P2P2", terminalDevices2);
  
  MobilityHelper mobility;
  Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();
  positionAlloc->Add (Vector (0.0, 0.0, 0.0));
  positionAlloc->Add (Vector (0.0, 10.0, 0.0));
  positionAlloc->Add (Vector (10.0, 10.0, 0.0));
    positionAlloc->Add (Vector (20.0, 10.0, 0.0));
  mobility.SetPositionAllocator (positionAlloc);
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (terminals);
  
  InternetStackHelper internet;
  internet.Install (terminals);
  
  NS_LOG_INFO ("Assign IP Addresses.");
  Ipv4AddressHelper address;
  address.SetBase ("10.1.1.0", "255.255.255.0");
  address.Assign (terminalDevices);
  address.Assign (terminalDevices1);
  address.Assign (terminalDevices2);
  
uint16_t flag=1;
while(flag<=2)
{
TypeId tid = TypeId::LookupByName ("ns3::UdpSocketFactory");
if(flag==1)
  {
  // instantiate a header.
   MyHeader sourceHeader;
   sourceHeader.SetData (521);
   
   // and store my header into the packet.
   p->AddHeader (sourceHeader);
  
  Ptr<Socket> recvSink = Socket::CreateSocket (terminals.Get (0), tid);
  InetSocketAddress local = InetSocketAddress (Ipv4Address::GetAny (), 80);
  recvSink->Bind (local);
  recvSink->SetRecvCallback (MakeCallback (&ReceivePacket));

  Ptr<Socket> source = Socket::CreateSocket (terminals.Get (1), tid);
  InetSocketAddress remote = InetSocketAddress (Ipv4Address ("255.255.255.255"), 80);
  source->SetAllowBroadcast (true);
  source->Connect (remote);
  
  Simulator::ScheduleWithContext (source->GetNode ()->GetId (),
                                  Seconds (1.0), &GenerateTraffic,
                                  source, p, packetSize, numPackets, interPacketInterval);


 }

 else if(flag==2)
 {
 // instantiate a header.
   MyHeader sourceHeader;
     
  sourceHeader.SetData (add);
   
  // and store my header into the packet.
  p->AddHeader(sourceHeader);
   
  Ptr<Socket> recvSink = Socket::CreateSocket (terminals.Get (3), tid);
  InetSocketAddress local = InetSocketAddress (Ipv4Address::GetAny (), 80);
  recvSink->Bind (local);
  recvSink->SetRecvCallback (MakeCallback (&ReceivePacket));

  Ptr<Socket> source = Socket::CreateSocket (terminals.Get (0), tid);
  InetSocketAddress remote = InetSocketAddress (Ipv4Address ("255.255.255.255"), 80);
  source->SetAllowBroadcast (true);
  source->Connect (remote);
  
  Simulator::ScheduleWithContext (source->GetNode ()->GetId (),
                                  Seconds (3.0), &GenerateTraffic,
                                  source, p, packetSize, numPackets, interPacketInterval);
                                  
 }
 flag=flag+1;
 }                                  
 
  AnimationInterface anim ("Sourabh_security.xml"); 
  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
 }
