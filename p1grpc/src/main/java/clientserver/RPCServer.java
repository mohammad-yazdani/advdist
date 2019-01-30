package clientserver;

import advdist.p1grpc.clientserver.*;
import io.grpc.Server;
import io.grpc.ServerBuilder;
import io.grpc.stub.StreamObserver;
import java.io.IOException;
import java.util.logging.Logger;

/**
 * Server that manages startup/shutdown of a {@code Greeter} server.
 */
public class RPCServer {
    private static final Logger logger = Logger.getLogger(RPCServer.class.getName());

    private Server server;

    private void start() throws IOException {
        /* The port on which the server should run */
        int port = 8080;
        server = ServerBuilder.forPort(port)
                .addService(new GreeterImpl())
                .addService(new StreamingGreeterImpl())
                .build()
                .start();
        logger.info("Server started, listening on " + port);
        Runtime.getRuntime().addShutdownHook(new Thread() {
            @Override
            public void run() {
                // Use stderr here since the logger may have been reset by its JVM shutdown hook.
                System.err.println("*** shutting down gRPC server since JVM is shutting down");
                RPCServer.this.stop();
                System.err.println("*** server shut down");
            }
        });
    }

    private void stop() {
        if (server != null) {
            server.shutdown();
        }
    }

    /**
     * Await termination on the main thread since the grpc library uses daemon threads.
     */
    private void blockUntilShutdown() throws InterruptedException {
        if (server != null) {
            server.awaitTermination();
        }
    }

    /**
     * Main launches the server from the command line.
     */
    public static void main(String[] args) throws IOException, InterruptedException {
        final RPCServer server = new RPCServer();
        server.start();
        server.blockUntilShutdown();
    }

    static class GreeterImpl extends GreeterGrpc.GreeterImplBase {

        @Override
        public void sayHello(HelloRequest req, StreamObserver<HelloReply> responseObserver) {
            HelloReply reply = HelloReply.newBuilder().setMessage(req.getName()).build();
            responseObserver.onNext(reply);
            responseObserver.onCompleted();
        }

        @Override
        public void transferFile(Payload req, StreamObserver<Ack> responseObserver) {
            Ack reply = Ack.newBuilder().setCode(String.valueOf(req.getBuffer().length())).build();
            responseObserver.onNext(reply);
            responseObserver.onCompleted();
        }
    }

    static class StreamingGreeterImpl extends StreamingGreeterGrpc.StreamingGreeterImplBase {

        @Override
        public StreamObserver<Payload> streamFile(StreamObserver<Ack> responseObserver) {
            System.out.println("Connecting stream observer");
            StreamObserver<Payload> so = new StreamObserver<Payload>() {
                @Override
                public void onNext(Payload value) {
                    System.out.println("onNext from server");
                    try {
                        Thread.sleep(1000L);
                    } catch (InterruptedException e) {
                        Thread.currentThread().interrupt();
                    }
                    responseObserver.onNext(Ack.getDefaultInstance());
                }

                @Override
                public void onError(Throwable t) {
                    System.out.println("on error");
                    t.printStackTrace();
                }

                @Override
                public void onCompleted() {
                    System.out.println("on completed");
                }
            };
            return so;
        }
    }
}