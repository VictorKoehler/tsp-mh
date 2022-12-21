# tsp-mh

Aluno: Victor José de Sousa Koehler

Matrícula: 20211023501



## Introdução

Neste trabalho são apresentadas implementações de modelos heurísticos para o Problema do Caixeiro-Viajante (do inglês, Travelling Salesman Problem), bem como algoritmos e estruturas auxiliares, como requisito da avaliação da disciplina de Tópicos Especiais do Programa de Pós-Graduação em Informática (PPGI) da Universidade Federal da Paraíba (UFPB). Esta implementação possui teor puramente educativo e não deve ser seriamente utilizada em ambientes de produção. Neste relatório são apresentados alguns resultados de experimentos computacionais realizados com esta implementação utilizando um conjunto de instâncias bem conhecido da literatura.

Na tabela abaixo, estão relacionados os resultados de experimentos para a implementação, deste trabalho, da Relaxação Lagrangeana, separadamente (coluna **root**) bem como aplicada em uma implementação Branch&Bound (coluna **tree**). **TIME** denota o tempo de execução, enquanto **lb** e **ub** o *lowerbound* e *upperbound*, respectivamente.

<table>
  <thead>
    <tr>
      <th >STAGE</th>
      <th colspan="3">root</th>
      <th colspan="3">tree</th>
    </tr>
    <tr>
      <th >&nbsp;</th>
      <th >TIME</th>
      <th >lb</th>
      <th >ub</th>
      <th >TIME</th>
      <th >lb</th>
      <th >ub</th>
    </tr>
    <tr>
      <th >NAME</th>
      <th >&nbsp;</th>
      <th >&nbsp;</th>
      <th >&nbsp;</th>
      <th >&nbsp;</th>
      <th >&nbsp;</th>
      <th >&nbsp;</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <th >att48.tsp</th>
      <td >0.04</td>
      <td >10313.40</td>
      <td >*</td>
      <td >0.93</td>
      <td >10628.00</td>
      <td >10628.00</td>
    </tr>
    <tr>
      <th >bayg29.tsp</th>
      <td >0.02</td>
      <td >1607.43</td>
      <td >*</td>
      <td >0.05</td>
      <td >1610.00</td>
      <td >1610.00</td>
    </tr>
    <tr>
      <th >bays29.tsp</th>
      <td >0.13</td>
      <td >2008.35</td>
      <td >*</td>
      <td >0.22</td>
      <td >2020.00</td>
      <td >2020.00</td>
    </tr>
    <tr>
      <th >berlin52.tsp</th>
      <td >0.01</td>
      <td >7444.77</td>
      <td >*</td>
      <td >1.41</td>
      <td >7542.00</td>
      <td >7542.00</td>
    </tr>
    <tr>
      <th >brazil58.tsp</th>
      <td >0.01</td>
      <td >22021.70</td>
      <td >*</td>
      <td >6.90</td>
      <td >25395.00</td>
      <td >25395.00</td>
    </tr>
    <tr>
      <th >burma14.tsp</th>
      <td >0.01</td>
      <td >3323.00</td>
      <td >*</td>
      <td >0.01</td>
      <td >3323.00</td>
      <td >3323.00</td>
    </tr>
    <tr>
      <th >cid5.tsp</th>
      <td >0.00</td>
      <td >148.00</td>
      <td >148.00</td>
      <td >0.00</td>
      <td >148.00</td>
      <td >148.00</td>
    </tr>
    <tr>
      <th >cidp5.tsp</th>
      <td >0.01</td>
      <td >148.00</td>
      <td >148.00</td>
      <td >0.01</td>
      <td >148.00</td>
      <td >148.00</td>
    </tr>
    <tr>
      <th >dantzig42.tsp</th>
      <td >0.01</td>
      <td >685.64</td>
      <td >*</td>
      <td >0.37</td>
      <td >699.00</td>
      <td >699.00</td>
    </tr>
    <tr>
      <th >eil51.tsp</th>
      <td >0.01</td>
      <td >418.32</td>
      <td >*</td>
      <td >35.61</td>
      <td >426.00</td>
      <td >426.00</td>
    </tr>
    <tr>
      <th >eil76.tsp</th>
      <td >0.02</td>
      <td >532.91</td>
      <td >*</td>
      <td >114.96</td>
      <td >538.00</td>
      <td >538.00</td>
    </tr>
    <tr>
      <th >fri26.tsp</th>
      <td >0.02</td>
      <td >936.95</td>
      <td >*</td>
      <td >0.03</td>
      <td >937.00</td>
      <td >937.00</td>
    </tr>
    <tr>
      <th >gr17.tsp</th>
      <td >0.00</td>
      <td >2085.00</td>
      <td >*</td>
      <td >0.00</td>
      <td >2085.00</td>
      <td >2085.00</td>
    </tr>
    <tr>
      <th >gr21.tsp</th>
      <td >0.00</td>
      <td >2707.00</td>
      <td >*</td>
      <td >0.00</td>
      <td >2707.00</td>
      <td >2707.00</td>
    </tr>
    <tr>
      <th >gr24.tsp</th>
      <td >0.01</td>
      <td >1271.87</td>
      <td >*</td>
      <td >0.01</td>
      <td >1272.00</td>
      <td >1272.00</td>
    </tr>
    <tr>
      <th >gr48.tsp</th>
      <td >0.01</td>
      <td >4879.79</td>
      <td >*</td>
      <td >1341.29</td>
      <td >5046.00</td>
      <td >5046.00</td>
    </tr>
    <tr>
      <th >gr96.tsp</th>
      <td >0.04</td>
      <td >53401.90</td>
      <td >*</td>
      <td >*</td>
      <td >*</td>
      <td >*</td>
    </tr>
    <tr>
      <th >hk48.tsp</th>
      <td >0.01</td>
      <td >11377.20</td>
      <td >*</td>
      <td >0.29</td>
      <td >11461.00</td>
      <td >11461.00</td>
    </tr>
    <tr>
      <th >kroA100.tsp</th>
      <td >0.04</td>
      <td >20732.70</td>
      <td >*</td>
      <td >*</td>
      <td >*</td>
      <td >*</td>
    </tr>
    <tr>
      <th >kroB100.tsp</th>
      <td >0.05</td>
      <td >21492.20</td>
      <td >*</td>
      <td >*</td>
      <td >*</td>
      <td >*</td>
    </tr>
    <tr>
      <th >kroC100.tsp</th>
      <td >0.04</td>
      <td >20127.30</td>
      <td >*</td>
      <td >*</td>
      <td >*</td>
      <td >*</td>
    </tr>
    <tr>
      <th >kroD100.tsp</th>
      <td >0.06</td>
      <td >20832.00</td>
      <td >*</td>
      <td >*</td>
      <td >*</td>
      <td >*</td>
    </tr>
    <tr>
      <th >kroE100.tsp</th>
      <td >0.06</td>
      <td >21449.40</td>
      <td >*</td>
      <td >*</td>
      <td >*</td>
      <td >*</td>
    </tr>
    <tr>
      <th >pr76.tsp</th>
      <td >0.02</td>
      <td >102735.00</td>
      <td >*</td>
      <td >*</td>
      <td >*</td>
      <td >*</td>
    </tr>
    <tr>
      <th >rat99.tsp</th>
      <td >0.05</td>
      <td >1170.86</td>
      <td >*</td>
      <td >*</td>
      <td >*</td>
      <td >*</td>
    </tr>
    <tr>
      <th >rd100.tsp</th>
      <td >0.05</td>
      <td >7859.89</td>
      <td >*</td>
      <td >6.60</td>
      <td >7910.00</td>
      <td >7910.00</td>
    </tr>
    <tr>
      <th >st70.tsp</th>
      <td >0.04</td>
      <td >660.95</td>
      <td >*</td>
      <td >3055.10</td>
      <td >675.00</td>
      <td >675.00</td>
    </tr>
    <tr>
      <th >swiss42.tsp</th>
      <td >0.01</td>
      <td >1246.17</td>
      <td >*</td>
      <td >0.44</td>
      <td >1273.00</td>
      <td >1273.00</td>
    </tr>
    <tr>
      <th >test10.tsp</th>
      <td >0.02</td>
      <td >803.00</td>
      <td >803.00</td>
      <td >0.02</td>
      <td >803.00</td>
      <td >803.00</td>
    </tr>
    <tr>
      <th >ulysses16.tsp</th>
      <td >0.02</td>
      <td >6858.92</td>
      <td >*</td>
      <td >0.02</td>
      <td >6859.00</td>
      <td >6859.00</td>
    </tr>
    <tr>
      <th >ulysses22.tsp</th>
      <td >0.01</td>
      <td >7006.13</td>
      <td >*</td>
      <td >0.01</td>
      <td >7013.00</td>
      <td >7013.00</td>
    </tr>
  </tbody>
</table>

