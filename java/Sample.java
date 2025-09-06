public class Sample {
    public static int sumToN(int n) {
        int acc = 0;
        for (int i = 1; i <= n; i++) {
            acc += i;
        }
        return acc;
    }

    public static void main(String[] args) {
        int n = 5;
        int s = sumToN(n);
        System.out.println("sumToN(" + n + ") = " + s);
    }
}
