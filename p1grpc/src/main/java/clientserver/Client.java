package clientserver;

import advdist.p1grpc.clientserver.GreeterGrpc;
import advdist.p1grpc.clientserver.HelloReply;
import advdist.p1grpc.clientserver.HelloRequest;
import io.grpc.ManagedChannel;
import io.grpc.ManagedChannelBuilder;
import io.grpc.StatusRuntimeException;
import stats.DataSet;

import java.io.BufferedWriter;
import java.io.FileWriter;
import java.util.Arrays;
import java.util.concurrent.TimeUnit;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 * A simple client that requests a greeting from the {@link RPCServer}.
 */
public class Client {
    private static final Logger logger = Logger.getLogger(Client.class.getName());

    private final ManagedChannel channel;
    private final GreeterGrpc.GreeterBlockingStub blockingStub;

    /** Construct client connecting to HelloWorld server at {@code host:port}. */
    public Client(String host, int port) {
        this(ManagedChannelBuilder.forAddress(host, port)
                // Channels are secure by default (via SSL/TLS). For the example we disable TLS to avoid
                // needing certificates.
                .usePlaintext()
                .build());
    }

    /** Construct client for accessing HelloWorld server using the existing channel. */
    Client(ManagedChannel channel) {
        this.channel = channel;
        blockingStub = GreeterGrpc.newBlockingStub(channel);
    }

    public void shutdown() throws InterruptedException {
        channel.shutdown().awaitTermination(5, TimeUnit.SECONDS);
    }

    /** Say hello to server. */
    public void greet(String name) {
        logger.info("Will try to greet " + name + " ...");
        HelloRequest request = HelloRequest.newBuilder().setName(name).build();
        HelloReply response;
        try {
            response = blockingStub.sayHello(request);
        } catch (StatusRuntimeException e) {
            logger.log(Level.WARNING, "RPC failed: {0}", e.getStatus());
            return;
        }
        logger.info("Greeting: " + response.getMessage());
    }

    /**
     * Greet server. If provided, the first element of {@code args} is the name to use in the
     * greeting.
     */
    public static void main(String[] args) throws Exception {
        Client client = new Client("18.221.69.86", 8080);
        BufferedWriter out = null;
        try {
            /* Access a service running on the local machine on port 50051 */
            String user = "world";
            if (args.length > 0) {
                user = args[0]; /* Use the arg as the name to greet if provided */
            }
            long totalStart = System.nanoTime();

            DataSet bench = new DataSet();

            for (int i = 0; i < 100; i++) {
                long start = System.nanoTime();
                client.greet(user + i);
                long end = System.nanoTime();
                bench.addData(end - start);
            }

            long totalEnd = System.nanoTime();
            System.out.println("TOTAL: " + (totalEnd - totalStart));
            for (long e : bench.getData()) {
                System.out.println(e);
            }

            double avg = bench.average();
            double per10 = bench.percentile(90);
            double per90 = bench.percentile(10);

            double[] sum = {avg, per10, per90};
            DataSet.saveToCsv(sum, "gRPCSummary");
            DataSet.saveToCsv(bench.getData(), "gRPCTotal");
        } finally {
            client.shutdown();
        }
    }
}
