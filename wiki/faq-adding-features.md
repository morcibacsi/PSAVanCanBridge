# Why is modifying or adding vehicle features through VAN/CAN difficult?

A common question is whether a specific vehicle feature can be added to the VAN-CAN Bridge, or whether the Bridge can modify the behavior of an existing control unit.

In theory, many vehicle functions can be influenced by changing the messages exchanged between control units. In practice, however, this is usually much more complicated than simply adding a few lines of code or sending a different CAN or VAN message.

There are several reasons for this.

## The feature may be on a different bus

Modern vehicles usually contain several communication networks.

For example, one group of control units may communicate on a VAN bus, while others use CAN, LIN, or another separate network. Even if two functions exist in the same vehicle, this does not mean their messages are visible on the same wires.

The VAN-CAN Bridge can only work with traffic that reaches the buses it is physically connected to, which is the comfort bus (CONF)

If the control unit responsible for a requested feature is located on another bus, the Bridge cannot simply see or modify those messages.

In that case, supporting the feature may require:

- connecting to an additional vehicle bus,
- additional hardware,
- another interface,
- or a different installation location.

So when a feature request sounds simple from the driver's point of view, it may actually involve a completely different part of the vehicle network.

## The message may not be known

Even when the relevant control unit is connected to a bus accessible to the Bridge, the communication is usually undocumented.

Before implementing a feature, it may first be necessary to determine:

- which control unit sends the relevant message,
- which control unit receives it,
- which message contains the function,
- which bytes or individual bits control it,
- when the message is sent,
- how often it is sent,
- and whether changing it affects other vehicle functions.

This normally requires recording bus traffic while operating the vehicle, comparing captures, and gradually reverse-engineering the communication.

Even understanding the basic behavior of one feature can take weeks or months of experimentation.

## Why can't the Bridge just send a different message?

This is one of the most common misunderstandings.

If the original control unit is already sending a message, transmitting another message with different data does not necessarily override it.

For example, the original ECU might continuously send:

`Function = OFF`

and the Bridge could try to send:

`Function = ON`

But both messages would then exist on the bus.

The receiving control unit may:

- immediately receive another `OFF`,
- ignore the additional message,
- reject it because of timing,
- expect the message from a particular source,
- or behave unpredictably because it receives conflicting information.

For some features, injecting an additional message works.

For many others, it does not.

## Sometimes the original message must be intercepted

If the original message needs to be changed, it may be necessary to prevent that message from reaching the destination at all.

This usually requires a **man-in-the-middle** setup.

For example:

`BSI <----> Interceptor <----> Target control unit`

Instead of connecting the two control units directly, an interceptor is physically inserted between them.

The interceptor receives each message and decides whether it should be:

- forwarded unchanged,
- modified before forwarding,
- or blocked completely.

Communication in the opposite direction may also need to be intercepted.

This is considerably more complicated than ordinary message injection because the interceptor has to behave transparently and keep the communication between both sides working correctly.

In some situations, two VAN-CAN Bridges or additional hardware could be used to create such a setup.

## Why can one feature be easy while another is extremely difficult?

From the user's point of view, two features may look very similar.

For example:

- displaying an additional value,
- changing how a button behaves,
- controlling a window,
- changing a dashboard indication,
- or automatically enabling a setting.

Internally, however, they may work completely differently.

One feature might only require transmitting one known message.

Another might require:

- discovering an undocumented message,
- accessing another vehicle bus,
- identifying multiple related messages,
- reproducing timing requirements,
- intercepting existing traffic,
- modifying communication in both directions,
- or reverse-engineering the behavior of several ECUs.

Because of this, the apparent complexity of the feature from the driver's perspective is not a good indication of how difficult it is to implement.

## Can you add a specific feature to the VAN-CAN Bridge?

Possibly, but there is usually no way to know without first understanding how that particular feature works in the vehicle.

If the necessary messages are already known and available on the buses connected to the Bridge, adding support may be relatively straightforward.

If they are not known, implementing the feature first becomes a reverse-engineering project.

If the feature exists on another vehicle bus, additional hardware or wiring may be required.

And if the feature requires replacing an existing message rather than simply sending an additional one, a man-in-the-middle setup may be necessary.

For these reasons, a request that sounds like a small software feature can sometimes turn into a substantial research and development project.

## What is the usual reverse-engineering process?

A typical process is:

1. Identify which control units are involved.
2. Determine which vehicle bus carries their communication.
3. Capture the bus traffic while the vehicle is operating normally.
4. Perform the action you are interested in, such as pressing a button or changing a setting.
5. Compare multiple captures and look for messages that change.
6. Narrow down the relevant frames.
7. Determine the meaning of the individual bytes and bits.
8. Check timing, counters, checksums, acknowledgements, and other protocol behavior if applicable.
9. Determine whether sending an additional message is enough.
10. If not, investigate whether the original communication must be intercepted.
11. Test carefully to make sure unrelated vehicle functions are not affected.

This is normally an iterative process and may require a considerable amount of testing.

## Can the VAN-CAN Bridge do this automatically?

No.

The VAN-CAN Bridge provides tools for communicating with the vehicle networks, but it does not automatically understand what every vehicle message means.

It cannot automatically determine:

- which message controls a requested feature,
- which bit needs to be changed,
- whether the message is on another bus,
- whether another ECU will overwrite the value,
- or whether a man-in-the-middle arrangement is required.

That knowledge has to come from reverse engineering, existing documentation, or previous research.

## Can you provide instructions or develop a specific custom feature?

Unfortunately, I cannot provide step-by-step reverse-engineering or development support for every individual vehicle feature request.

These projects can require a large amount of investigation, testing, vehicle-specific knowledge, and development time. In some cases, several months can be spent just understanding the basic communication involved.

The VAN-CAN Bridge is intended to provide the tools needed for this kind of experimentation and development, but researching and implementing individual vehicle-specific features is generally outside the scope of product support.

The current capabilities of the VAN-CAN Bridge are the result of many years of development and reverse engineering, with work on the project having started around 2016. Features that may look simple today often depend on knowledge and experimentation accumulated over a long period of time.