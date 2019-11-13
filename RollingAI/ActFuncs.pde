interface ActivationFunction
{
  float compute(float x);
}

class Linear implements ActivationFunction
{
  float compute(float x)
  {
    return x;
  }
}

class Sigmoid implements ActivationFunction
{
  float compute(float x)
  {
    return (2 / (1 + exp(-x))) - 1;
  }
}

class Tipping implements ActivationFunction
{
  float compute(float x)
  {
    return 5*(pow(x/5,3));
  }
}

class Composite implements ActivationFunction
{
  ActivationFunction f1, f2;
  
  Composite(ActivationFunction _f1, ActivationFunction _f2)
  {
    f1 = _f1;
    f2 = _f2;
  }
  
  float compute(float x)
  {
    return f1.compute(f2.compute(x));
  }
}

class Inverse implements ActivationFunction
{
  float numerator;
  
  Inverse(float _numerator)
  {
    numerator = _numerator;
  }
  
  float compute(float x)
  {
    return numerator/x;
  }
}

class ReLU implements ActivationFunction{
  float compute(float x)
  {
    return x < 0 ? 0 : x;
  }
  
}
