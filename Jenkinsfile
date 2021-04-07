@Library('eo-jenkins-lib@main') import eo.Utils

pipeline {
    agent any
    stages {
        stage('Unit Tests') {
            steps {
                sh './jenkins.sh'
            }
        }
        stage('Packaging') {
            when {
                expression {
                   currentBuild.result == null || currentBuild.result == 'SUCCESS'
                }
                anyOf {
                   tag 'v*'
                   branch 'main'
                }
            }
            steps {
                script {
                    sh 'sudo -H -u eobuilder /usr/local/bin/eobuilder lasso'
                }
            }
        }
    }
    post {
        success {
            cleanWs()
        }
    }
}
