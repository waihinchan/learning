using UnityEngine;

public class RotateTest : MonoBehaviour
{
    //This is the Transform of the second GameObject
    public Vector3 dir1;
    public Vector3 dir2;
    private MeshFilter mf;
    private Vector3[] origVerts;
    private Vector3[] newVerts;
    void Start()
    {
        mf = GetComponent<MeshFilter>();
        origVerts = mf.mesh.vertices;
        newVerts = new Vector3[origVerts.Length];
    }

    void Update()
    {   
        Vector3 normalizeDir1 =  Vector3.Normalize(dir1); //归一化之后变成1
        Vector3 normalizeDir2 =  Vector3.Normalize(dir2); //归一化之后变成1
        Matrix4x4 m = GetRotateMartixFromVector(normalizeDir1,normalizeDir2);



        Debug.DrawLine(new Vector3(0,0,0),dir2);
        int i = 0;
        while (i < origVerts.Length) {
            newVerts[i] = m.MultiplyVector(origVerts[i]);
            i++;
        }
        mf.mesh.vertices = newVerts;
        Vector3 result = m.MultiplyPoint3x4(normalizeDir1);
        result = Vector3.Normalize(result);
        Debug.Log(Vector3.Dot(result,normalizeDir2));
    }
    Matrix4x4 GetRotateMartixFromVector(Vector3 _dir1,Vector3 _dir2){
        Vector3 rotateionAxis =  Vector3.Cross(_dir1,_dir2); //叉乘获得旋转轴
        float sinTheta = rotateionAxis.magnitude; //模长 = dir1 * dir2 * sin, dir1、2 都是1,直接取即可
        float cosTheta = Vector3.Dot(_dir1,_dir2); //点乘直接获得cos
        float u = rotateionAxis.x;
        float v = rotateionAxis.y;
        float w = rotateionAxis.z;
        Matrix4x4 m = new Matrix4x4();

        m[0,0] =      cosTheta + u*u*(1-cosTheta);
        m[1,0] =  w * sinTheta + v*u*(1-cosTheta); 
        m[2,0] = -v * sinTheta + w*u*(1-cosTheta); 
        m[3,0] = 0;

        m[0,1] = -w * sinTheta + u*v*(1-cosTheta);
        m[1,1] =     cosTheta + v*v*(1-cosTheta);
        m[2,1] =  u * sinTheta + w*v*(1-cosTheta);
        m[3,1] = 0;

        m[0,2] =  v * sinTheta + u*w*(1-cosTheta);
        m[1,2] = -u * sinTheta + v*w*(1-cosTheta);
        m[2,2] =      cosTheta + w*w*(1-cosTheta);
        m[3,2] = 0;

        m[0,3] = 0;
        m[1,3] = 0;
        m[2,3] = 0;
        m[3,3] = 1;

        return m;

    }
}
