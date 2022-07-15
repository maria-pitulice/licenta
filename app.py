import json
from os.path import join, dirname, realpath
from flask import Flask, render_template, request
from werkzeug.utils import secure_filename
from algorithms import compare2Files, removeBlankLines, getTextFromFile, randomRGB, getLanguage, getFingerprints, getPercentage
from os import listdir, remove
from json import loads
import pyodbc
import time

K=5
w=4
cnxn = pyodbc.connect('DRIVER={ODBC Driver 17 for SQL Server};SERVER=localhost;DATABASE=antiplagiat;Trusted_Connection=yes;')
cursor = cnxn.cursor()

app = Flask(__name__)
app.config['UPLOAD_FOLDER'] = join(dirname(realpath(__file__)), 'uploads')


@app.route('/')
def index():
    return render_template('index.html')


@app.route('/list', methods=['POST'])
def url_list():
    start_time = time.time()
    files = request.files.getlist('file')
    for file in files:
        path = join(app.config['UPLOAD_FOLDER'], secure_filename(file.filename))
        file.save(path)
        #removeBlankLines(path)
    filenames = [secure_filename(x.filename) for x in files]
    filepaths = [join(app.config['UPLOAD_FOLDER'], filename) for filename in filenames]
    query_string = 'delete from dbo.fingerprints where filename in (%s)' % ','.join(['?'] * len(filepaths))
    cursor.execute(query_string, filepaths)
    cursor.commit()
    fingerprints = [str(getFingerprints(filepath, getLanguage(filepath), K, w)) for filepath in filepaths]
    cursor.executemany('insert into dbo.fingerprints(filename, fingerprint) values (?, ?)',zip(filenames, fingerprints))
    cursor.commit()
    cursor.execute('select * from dbo.fingerprints')
    prints_to_file = {x[0]: json.loads(x[1]) for x in list(cursor.fetchall()) }
    results1 = []
    percentages1 = []

    intermed_time=time.time()

    for i in range(len(filenames)):
        results_row = []
        percentages_row = []
        path1 = join(app.config['UPLOAD_FOLDER'], secure_filename(filenames[i]))
        fingerprint1 = prints_to_file[filenames[i]]
        for j in range(len(filenames)):
            path2 = join(app.config['UPLOAD_FOLDER'], secure_filename(filenames[j]))
            if j <= i or getLanguage(path1) != getLanguage(path2):
                results_row.append([])
                percentages_row.append([])
                continue
            fingerprint2 = prints_to_file[filenames[j]]
            lines1, lines2 = compare2Files(fingerprint1, fingerprint2,K)
            results_row.append([lines1, lines2])
            percentages_row.append(getPercentage(lines1, lines2, path1, path2))
        results1.append(results_row)
        percentages1.append(percentages_row)
    
    end_time=time.time()
    seconds=end_time-start_time
    intermed_finger=intermed_time-start_time
    intermed_compare=end_time-intermed_time
    return render_template('list.html', files=filenames,
                           results1=results1, percentages1=percentages1, seconds=round(seconds,2),intermedF=round(intermed_finger,2),intermedC=round(intermed_compare,2))


@app.route('/result', methods=['POST'])
def result():
    filename1 = request.args['filename1']
    filename2 = request.args['filename2']
    data = loads(request.form['data'])
    path1 = join(app.config['UPLOAD_FOLDER'], secure_filename(filename1))
    path2 = join(app.config['UPLOAD_FOLDER'], secure_filename(filename2))
    lines1, lines2 = data
    text1 = getTextFromFile(path1)
    text2 = getTextFromFile(path2)
    colors1 = {}
    colors2 = {}
    corespond_to_color={}
    for i in lines1:
        if i[1] not in corespond_to_color: #daca corespondenta e deja in dictionar, culoarea ramane aceeasi, nu se actualizeaza random 
            corespond_to_color.update({i[1]:randomRGB()})
    for i in lines1:
        colors1.update({i[0]:corespond_to_color.get(i[1])})
    for i in lines2:
        colors2.update({i[0]:corespond_to_color.get(i[1])})
   

    percentage = int(min(len(lines1), len(lines2)) / min(len(text1), len(text2)) * 100)

    return render_template('result.html', text1=text1, text2=text2, lines1=lines1, lines2=lines2,
                percentage=percentage, colors1=colors1, colors2=colors2, filename1=filename1, filename2=filename2)



app.run(host='0.0.0.0', port=81, debug=True)