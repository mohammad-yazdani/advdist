package stats;

import org.apache.commons.math3.stat.descriptive.rank.Percentile;

import java.io.BufferedWriter;
import java.io.FileWriter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Collections;

public class DataSet {
    private ArrayList<Long> data;

    public DataSet() {
        this.data = new ArrayList<>();
    }

    public void addData(long e) throws Exception {
        if(!this.data.add(e)) throw new Exception("Couldn't add to data set.");
    }

    public ArrayList<Long> getData() {
        return data;
    }

    public double average() {
        double sum = 0;
        for (long e : this.data) {
            sum += (double) e;
        }
        double quanta = (double) this.data.size();
        return sum / quanta;
    }

    public double percentile(double percent) {
        ArrayList<Double> sortedCopy = new ArrayList<>();
        for (long e : this.data) {
            sortedCopy.add((double) e);
        }
        Collections.sort(sortedCopy);

        double[] sortedArr = new double[sortedCopy.size()];
        for (int i = 0; i < sortedCopy.size(); i++) {
            sortedArr[i] = sortedCopy.get(i);
        }

        Percentile p = new Percentile();
        return p.evaluate(sortedArr, percent);
    }

    public static void saveToCsv(double[] data, String name) throws IOException {
        BufferedWriter out;
        StringBuilder buffer = new StringBuilder();
        FileWriter fileWriter = new FileWriter(name + ".csv", false);
        out = new BufferedWriter(fileWriter);
        for (double e : data) {
            buffer.append(String.format("%.12f", e)).append("\n");
        }
        out.write(buffer.toString());
        out.close();
    }

    public static void saveToCsv(ArrayList<Long> data, String name) throws IOException {
        BufferedWriter out;
        StringBuilder buffer = new StringBuilder();
        FileWriter fileWriter = new FileWriter(name + ".csv", false);

        out = new BufferedWriter(fileWriter);
        for (long e : data) {
            buffer.append(String.format("%08d", e)).append("\n");
        }
        out.write(buffer.toString());
        out.close();
    }

    public void save(String name) throws IOException {
        double avg = this.average();
        double per10 = this.percentile(90);
        double per90 = this.percentile(10);

        double[] sum = {avg, per10, per90};
        DataSet.saveToCsv(sum, name + "Summary");
        DataSet.saveToCsv(this.getData(), name + "Total");
    }
}
