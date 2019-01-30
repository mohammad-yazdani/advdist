package clientserver;

import advdist.p1grpc.clientserver.*;
import io.grpc.ManagedChannel;
import io.grpc.ManagedChannelBuilder;
import io.grpc.StatusRuntimeException;
import stats.DataSet;

import java.io.File;
import java.io.RandomAccessFile;
import java.nio.MappedByteBuffer;
import java.nio.channels.FileChannel;
import java.nio.file.Path;
import java.nio.file.Paths;
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

    public void transfer(String buffer) {
        Payload payload = Payload.newBuilder().setBuffer(buffer).build();
        Ack response;
        try {
            response = blockingStub.transferFile(payload);
        } catch (StatusRuntimeException e) {
            logger.log(Level.WARNING, "RPC failed: {0}", e.getStatus());
        }
    }

    private static void experiment1(Client client) throws Exception {
        String user = "world";
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

        bench.save("gRPC");
    }

    private static void experiment2(Client client) throws Exception {
        Path currentRelativePath = Paths.get("");
        String s = currentRelativePath.toAbsolutePath().toString();
        DataSet bench = new DataSet();
        String largeFileName = s + "/src/main/resources/10mb.txt";

        for (int i = 0; i < 100; i++) {
            File f = new File(largeFileName);
            FileChannel fc = new RandomAccessFile(f, "rw").getChannel();
            MappedByteBuffer mappedByteBuffer = fc.map(FileChannel.MapMode.READ_ONLY, 0, f.length());

            mappedByteBuffer.load();
            int chunkSize = 1048576;

            String buf = mappedByteBuffer.asReadOnlyBuffer().asCharBuffer().toString();
            long start = System.nanoTime();
            String chunk = buf.substring(0, chunkSize);
            client.transfer(chunk);
            long end = System.nanoTime();
            bench.addData(end - start);
            fc.close();
            System.out.println("ITERATION " + i);
        }

        bench.save("gRPCLarge");
    }

    /**
     * Greet server. If provided, the first element of {@code args} is the name to use in the
     * greeting.
     */
    public static void main(String[] args) throws Exception {
        Client client = new Client("18.221.69.86", 8080);
        try {
            experiment1(client);
            experiment2(client);
        } finally {
            client.shutdown();
        }
    }
}
