#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
NBA point guard statistics mining.

@author: flora
"""

import pandas as pd
import seaborn as sns
import matplotlib.pyplot as plt
import numpy as np

#import statsmodels.api as sm
#import statsmodels.formula.api as smf

# TODO:
#season = 18 # the 20(x-1) - 20x
for season in range(14, 20):
    filename1 = 'pg_per-100-poss/'+str(season)+'.csv'
    filename2 = 'pg_per-100-poss/Advanced/Advanced_'+str(season)+'.csv'
    filename3 = 'pg_rpm/RPM-PG_20'+str(season-1)+'-20'+str(season)+'.csv'
    
    per_poss = pd.read_csv(filename1, delimiter=',', encoding='utf-8')
    per_poss_some = per_poss.copy()
    per_poss_some.drop(['Rk','Pos','Age','Tm','G','GS','MP','TRB'], 
                       inplace=True, axis=1)
    
    per_poss_adv = pd.read_csv(filename2, delimiter=',', encoding='utf-8')
    per_poss_adv_some = per_poss_adv.copy()
    per_poss_adv_some.drop(['Rk','Pos','Age','Tm','G','MP','TRB%'], 
                           inplace=True, axis=1)
    
    rpm = pd.read_csv(filename3, delimiter=',', encoding='utf-8')
    rpm_some = rpm[['NAME','ORPM','DRPM','RPM','WINS']].copy()
    rpm_some.rename(columns={'NAME':'Player','WINS':'RPM_WINS'}, inplace=True)
    
    conn_basic = per_poss_some.merge(rpm_some, how='inner', on='Player')
    conn_adv = per_poss_adv_some.merge(rpm_some, how='inner', on='Player')
    
    # There does appear to be a stronger correlation between 
    # defensive rebounds and steals, and a player's RPM. 
    # Now let's visualize this relationship.
    corrMatrix = conn_adv.corr(method='pearson') #spearman
    '''
    plt.subplots(figsize=(20,15))
    plt.axes().set_title("NBA Point Guard Correlation Heatmap: 20" +
            str(season-1)+'-20'+str(season)+" Season (STATS & RPMS)")
    sns.heatmap(corrMatrix,
                vmin=-1, vmax=1, center=0,
                cmap=sns.diverging_palette(20, 220, n=200),
                square=True) #annot=True, cmap='Reds' 'Blues'
    '''
    # How about a covariance summary?
    covMatrix = conn_basic.cov()
    '''
    plt.subplots(figsize=(20,15))
    plt.axes().set_title("NBA Point Guard Covariance Heatmap: 20" +
            str(season-1)+'-20'+str(season)+" Season (STATS & RPMS)")
    sns.heatmap(covMatrix,
                vmin=-1, vmax=1, center=0,
                cmap=sns.diverging_palette(20, 220, n=200),
                square=True) #annot=True, cmap='Reds' 'Blues'
    
    #Now let's built out a regression model, to identify the strength of this predictive model.
    #defense = smf.ols('WINS_RPM ~ DRB + STL', data=player_df_def).fit()
    #print(defense.summary())
    
    # Let's run another set of regression diagnostics, to further assess the model.
    sns.jointplot("STL%", "WS", data=conn_adv, size=10, ratio=2, color="r")
    # When isolating steals as a sole predictor of a player's RPM, 
    # we can see that there is still a positive correlation.
    
    # Export to csv for interactive visualzation
    corrMatrix.to_csv("corrMatrix/corrMatrix_"+str(season)+".csv")
    covMatrix.to_csv("covMatrix/covMatrix_"+str(season)+".csv")
    '''
    # Export reshaped data for heatmap directly
    matrix = [[corrMatrix,'corrMatrix'],[covMatrix,'covMatrix']]
    for l in matrix:
        m = l[0]
        n = l[1]
        reshape_list = []
        column = m.columns.tolist()
        for x in column:
            for y in column:
                value = m.loc[x,y]
                row = [x,y,value]
                reshape_list.append(row)
        reshape_df = pd.DataFrame(data=np.array(reshape_list),columns=['x','y','value'])
        reshape_df.to_csv(n + "/" + n + "_reshape_"+str(season)+".csv",index=False)