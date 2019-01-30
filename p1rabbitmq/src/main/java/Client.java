import com.rabbitmq.client.Channel;
import com.rabbitmq.client.Connection;
import com.rabbitmq.client.ConnectionFactory;
import stats.DataSet;

import java.io.BufferedReader;
import java.io.FileReader;
import java.nio.file.Path;
import java.nio.file.Paths;

public class Client {

    private final static String QUEUE_NAME = "q0";

    private static void large() {
        Path currentRelativePath = Paths.get("");
        String s = currentRelativePath.toAbsolutePath().toString();
        DataSet bench = new DataSet();
        String largeFileName = s + "/src/main/resources/10mb.txt";

        ConnectionFactory factory = new ConnectionFactory();
        factory.setHost("18.221.69.86");
        try {
            Connection connection = factory.newConnection();
            Channel channel = connection.createChannel();
            channel.queueDeclare(QUEUE_NAME, false, false, false, null);

            for (int i = 0; i < 100; i++) {
                String buffer;
                try(BufferedReader br = new BufferedReader(new FileReader(largeFileName))) {
                    StringBuilder sb = new StringBuilder();
                    String line = br.readLine();

                    while (line != null) {
                        sb.append(line);
                        sb.append(System.lineSeparator());
                        line = br.readLine();
                    }
                    buffer = sb.toString();
                }

                long start = System.nanoTime();
                channel.basicPublish("", QUEUE_NAME, null, buffer.getBytes());
                long end = System.nanoTime();
                bench.addData(end - start);
                System.out.println("ITERATION " + i);
            }

            bench.save("RabbitMQLarge");
        } catch (Exception e) {
            System.out.println(e.getMessage());
        }
    }

    private static void simple() {
        ConnectionFactory factory = new ConnectionFactory();
        factory.setHost("18.221.69.86");
        try {
            Connection connection = factory.newConnection();
            Channel channel = connection.createChannel();
            channel.queueDeclare(QUEUE_NAME, false, false, false, null);

            String message;
            DataSet bench = new DataSet();
            for (int i = 0; i < 100; i++) {
                long start = System.nanoTime();
                message = String.valueOf(i);
                channel.basicPublish("", QUEUE_NAME, null, message.getBytes());
                long end = System.nanoTime();
                System.out.println("[" + i + "] Sent '" + message + "'");
                bench.addData(end - start);
            }
            bench.save("RabbitMQSimple");
            System.out.println("END");
        } catch (Exception e) {
            System.out.println(e.getMessage());
        }
    }

    public static void main(String[] argv) {
        Client.simple();
        Client.large();
    }
}
