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



class Linear implements ActivationFunction
{
  float compute(float x)
  {
    return x;
  }
}

class Sigmoid implements ActivationFunction
{
  int limit, div, low;
  Sigmoid(int _limit, int _div, int _low){
    limit = _limit;
    div = _div;
    low = _low;
  }
  
  float compute(float x)
  {
    return (limit / (1 + exp((-x)/div))) - low;
  }
}

class Tipping implements ActivationFunction
{
  
  float coeff, div, poly;
  Tipping(float c, int d, int p){
    coeff = c;
    div = d;
    poly = p;
  }
  
  float compute(float x)
  {
    return coeff*(pow(x/div, poly));
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

class ReLU implements ActivationFunction {
  float compute(float x)
  {
    return x < 0 ? 0 : x;
  }
}
class HyperTangent implements ActivationFunction {
  float compute(float x)
  {
    float cosh = (exp(x)+exp(-x))/2;
    float sinh = (exp(x)-exp(-x))/2;
    return sinh/cosh;
  }
}
