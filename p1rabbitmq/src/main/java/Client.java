import com.rabbitmq.client.Channel;
import com.rabbitmq.client.Connection;
import com.rabbitmq.client.ConnectionFactory;
import stats.DataSet;

public class Client {

    private final static String QUEUE_NAME = "q0";

    public static void main(String[] argv) {
        ConnectionFactory factory = new ConnectionFactory();
        factory.setHost("18.221.69.86");
        try {
            Connection connection = factory.newConnection();
            Channel channel = connection.createChannel();

            channel.queueDeclare(QUEUE_NAME, false, false, false, null);
            String message = "Hello World!";

            DataSet bench = new DataSet();
            for (int i = 0; i < 100; i++) {
                long start = System.nanoTime();
                channel.basicPublish("", QUEUE_NAME, null, message.getBytes());
                long end = System.nanoTime();
                System.out.println(" [x] Sent '" + message + "'");
                bench.addData(end - start);
            }
            bench.save("RabbitMQSimple");

        } catch (Exception e) {
            System.out.println(e.getMessage());
        }
    }
}
