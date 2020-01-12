#!/usr/bin/env python2
# -*- coding: utf-8 -*-
"""
Created on Fri Dec 27 08:16:50 2019

@author: flora
"""

import requests
import lxml.html as lh
import pandas as pd

url0 = 'http://www.espn.com/nba/statistics/rpm/_/year/'
for year in range(2014,2020):
    urls = []
    urls.append(url0 + str(year) + '/position/1')

    #Create a handle, page, to handle the contents of the website
    page = requests.get(urls[0])

    #Store the contents of the website under content
    doc = lh.fromstring(page.content)

    #Check if there are more than one page
    page_numbers = doc.find_class('page-numbers')
    page_num_str = page_numbers[0].text_content()
    if page_num_str[-1] != '1':
        urls.append(url0 + str(year) + '/page/2/position/1')

    col = []
    for i,url in enumerate(urls):
        if i == 1:
            #Create a handle, page, to handle the contents of the website
            page = requests.get(url)
    
            #Store the contents of the website under content
            doc = lh.fromstring(page.content)

        #Parse data that are stored between <tr>..</tr> of HTML
        tr_elements = doc.xpath('//tr')

        #Check the length of the first 20 rows
        #print([len(T) for T in tr_elements[:20]])

        #Parse table header
        if i == 0:
            for t in tr_elements[0]:
                header = t.text_content()
                col.append((header, []))

        #Since out first row is the header, data is stored on the second row onwards
        for j in range(1,len(tr_elements)):
            R = tr_elements[j]

            #If row is not of size 10, the //tr data is not from our table 
            if len(R) != 9:
                break

            #Iterate through each element of the row
            for k,t in enumerate(R.iterchildren()):
                data = t.text_content()
                #Append the data to the empty list of the i'th column
                col[k][1].append(data)
            #print(j,col[1][1])

    Dict = {title:column for (title,column) in col}
    df = pd.DataFrame(Dict)
    columns = [col[i][0] for i in range(len(col))]
    df.to_csv('RPM-PG_'+str(year-1)+'-'+str(year)+'.csv',sep=',',index=False,columns=columns)