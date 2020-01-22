interface ActivationFunction
{
  float compute(float x);
}

class Tilt implements ActivationFunction
{
  float compute(float x)
  {
    return x;
  }
}

class Edge implements ActivationFunction
{
  float compute(float x)
  {
    return pow(x,3);
  }
}

class Cntr implements ActivationFunction
{
  float compute(float x)
  {
    return 2/(1+exp(-5*x))-1;
  }
}
