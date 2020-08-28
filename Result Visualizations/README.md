<li>VisualAnalysisByCategory.ipynb - A Jupyter Notebook with analysis of experimental results at the end of Phase 1. In this experiment, 11 language pairs were procured, and each was removed and generated using the other 10 independently. Results were analyzed for different POS categories based on 2 metrics: Percentage of verifiably correct predictions from excluded data, Percentage of predictions as compared to excluded data. Quantiative results and Qualitative inferences are both presented. <br>
<li>DetailedVisualAnalysisbyCat.ipynb - A Jupyter Notebook which shows a better and finer analysis and visualization of results of the above experiment, by dividing errors into 4 categories based on the number of words (out of 2) of a translation found in data, both for precision and recall. This gives a much more realistic picture of the performance. Explanation of the formulas are further provided here: https://hackmd.io/@o7NQTGl-SCCmH18WdRRuzA/r1A40BXyw <br>

An html file is provided with both for a quick-view of the latest results generated. If this doesn't suffice, download the Jupyter Notebook using the following instructions:

<details>
	<summary> Instructions for viewing the notebook in a tweakable manner </summary>
  <ol>
	<li> Open the notebook in Github: https://github.com/shash42/ApertiumBidixGen/blob/master/DetailedVisualAnalysisbyCat.ipynb
	<li> Click on Download in the top-right.
	<li> Install plotly: (https://plotly.com/python/getting-started/#installation) :  pip install plotly==4.8.2
	<li> Install numpy:  pip install numpy
	<li> Install Jupyter Notebook: pip install "notebook>=5.3" "ipywidgets>=7.2"
  <li> After that you can access the Jupyter Notebook as you would do normally on your system: <br>
        Command to open jupyter through terminal: jupyter notebook <br>
Jupyter notebook will open in your browser tab (or as configured otherwise) and then navigate to the location of the file downloaded from the first link-above and open it. Use Kernel -> Restart and Run All if the visualizations don't show up directly.
  </ol>
</details>
</ul>
