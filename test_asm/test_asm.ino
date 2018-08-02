byte n1 = 2;
byte n2 = 2;
byte sum = 0;

void setup() {
  Serial.begin(9600);
  // put your setup code here, to run once:
  asm (
    "add %1, %2 \n"
    : "=r" (sum) : "r" (n1), "r" (n2)
  );
  Serial.print("n1: ");
  Serial.println(n1);
  Serial.print("n2: ");
  Serial.println(n2);
  Serial.print("sum: ");
  Serial.println(sum);
}

void loop() {
  // put your main code here, to run repeatedly:

}
